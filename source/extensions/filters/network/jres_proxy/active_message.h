#pragma once

#include "envoy/event/deferred_deletable.h"
#include "envoy/network/connection.h"
#include "envoy/network/filter.h"
#include "envoy/stats/timespan.h"

#include "common/buffer/buffer_impl.h"
#include "common/common/linked_object.h"
#include "common/common/logger.h"
#include "common/stream_info/stream_info_impl.h"

#include "extensions/filters/network/jres_proxy/decoder.h"
#include "extensions/filters/network/jres_proxy/decoder_event_handler.h"
#include "extensions/filters/network/jres_proxy/filters/filter.h"
#include "extensions/filters/network/jres_proxy/metadata.h"
#include "extensions/filters/network/jres_proxy/router/router.h"
#include "extensions/filters/network/jres_proxy/stats.h"

#include "absl/types/optional.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {

class ConnectionManager;
class ActiveMessage;

class ActiveResponseDecoder : public ResponseDecoderCallbacks,
                              public StreamHandler,
                              Logger::Loggable<Logger::Id::jres> {
public:
  ActiveResponseDecoder(ActiveMessage& parent, JresFilterStats& stats,
                        Network::Connection& connection, ProtocolPtr&& protocol);
  ~ActiveResponseDecoder() override = default;

  JresFilters::UpstreamResponseStatus onData(Buffer::Instance& data);

  // StreamHandler
  void onStreamDecoded(MessageMetadataSharedPtr metadata, ContextSharedPtr ctx) override;

  // ResponseDecoderCallbacks
  StreamHandler& newStream() override { return *this; }
  void onHeartbeat(MessageMetadataSharedPtr) override { NOT_IMPLEMENTED_GCOVR_EXCL_LINE; }

  uint64_t requestId() const { return metadata_ ? metadata_->requestId() : 0; }

private:
  FilterStatus applyMessageEncodedFilters(MessageMetadataSharedPtr metadata, ContextSharedPtr ctx);

  ActiveMessage& parent_;
  JresFilterStats& stats_;
  Network::Connection& response_connection_;
  ProtocolPtr protocol_;
  ResponseDecoderPtr decoder_;
  MessageMetadataSharedPtr metadata_;
  bool complete_ : 1;
  JresFilters::UpstreamResponseStatus response_status_;
};

using ActiveResponseDecoderPtr = std::unique_ptr<ActiveResponseDecoder>;

class ActiveMessageFilterBase : public virtual JresFilters::FilterCallbacksBase {
public:
  ActiveMessageFilterBase(ActiveMessage& parent, bool dual_filter)
      : parent_(parent), dual_filter_(dual_filter) {}
  ~ActiveMessageFilterBase() override = default;

  // JresFilters::FilterCallbacksBase
  uint64_t requestId() const override;
  uint64_t streamId() const override;
  const Network::Connection* connection() const override;
  JresProxy::Router::RouteConstSharedPtr route() override;
  SerializationType serializationType() const override;
  ProtocolType protocolType() const override;
  StreamInfo::StreamInfo& streamInfo() override;
  Event::Dispatcher& dispatcher() override;
  void resetStream() override;

protected:
  ActiveMessage& parent_;
  const bool dual_filter_ : 1;
};

// Wraps a DecoderFilter and acts as the DecoderFilterCallbacks for the filter, enabling filter
// chain continuation.
class ActiveMessageDecoderFilter : public JresFilters::DecoderFilterCallbacks,
                                   public ActiveMessageFilterBase,
                                   public LinkedObject<ActiveMessageDecoderFilter>,
                                   Logger::Loggable<Logger::Id::jres> {
public:
  ActiveMessageDecoderFilter(ActiveMessage& parent, JresFilters::DecoderFilterSharedPtr filter,
                             bool dual_filter);
  ~ActiveMessageDecoderFilter() override = default;

  void continueDecoding() override;
  void sendLocalReply(const JresFilters::DirectResponse& response, bool end_stream) override;
  void startUpstreamResponse() override;
  JresFilters::UpstreamResponseStatus upstreamData(Buffer::Instance& buffer) override;
  void resetDownstreamConnection() override;

  JresFilters::DecoderFilterSharedPtr handler() { return handle_; }

private:
  JresFilters::DecoderFilterSharedPtr handle_;
};

using ActiveMessageDecoderFilterPtr = std::unique_ptr<ActiveMessageDecoderFilter>;

// Wraps a EncoderFilter and acts as the EncoderFilterCallbacks for the filter, enabling filter
// chain continuation.
class ActiveMessageEncoderFilter : public ActiveMessageFilterBase,
                                   public JresFilters::EncoderFilterCallbacks,
                                   public LinkedObject<ActiveMessageEncoderFilter>,
                                   Logger::Loggable<Logger::Id::jres> {
public:
  ActiveMessageEncoderFilter(ActiveMessage& parent, JresFilters::EncoderFilterSharedPtr filter,
                             bool dual_filter);
  ~ActiveMessageEncoderFilter() override = default;

  void continueEncoding() override;
  JresFilters::EncoderFilterSharedPtr handler() { return handle_; }

private:
  JresFilters::EncoderFilterSharedPtr handle_;

  friend class ActiveMessage;
};

using ActiveMessageEncoderFilterPtr = std::unique_ptr<ActiveMessageEncoderFilter>;

// ActiveMessage tracks downstream requests for which no response has been received.
class ActiveMessage : public LinkedObject<ActiveMessage>,
                      public Event::DeferredDeletable,
                      public StreamHandler,
                      public JresFilters::DecoderFilterCallbacks,
                      public JresFilters::FilterChainFactoryCallbacks,
                      Logger::Loggable<Logger::Id::jres> {
public:
  ActiveMessage(ConnectionManager& parent);
  ~ActiveMessage() override;

  // Indicates which filter to start the iteration with.
  enum class FilterIterationStartState { AlwaysStartFromNext, CanStartFromCurrent };

  // Returns the encoder filter to start iteration with.
  std::list<ActiveMessageEncoderFilterPtr>::iterator
  commonEncodePrefix(ActiveMessageEncoderFilter* filter, FilterIterationStartState state);
  // Returns the decoder filter to start iteration with.
  std::list<ActiveMessageDecoderFilterPtr>::iterator
  commonDecodePrefix(ActiveMessageDecoderFilter* filter, FilterIterationStartState state);

  // Jres::FilterChainFactoryCallbacks
  void addDecoderFilter(JresFilters::DecoderFilterSharedPtr filter) override;
  void addEncoderFilter(JresFilters::EncoderFilterSharedPtr filter) override;
  void addFilter(JresFilters::CodecFilterSharedPtr filter) override;

  // StreamHandler
  void onStreamDecoded(MessageMetadataSharedPtr metadata, ContextSharedPtr ctx) override;

  // JresFilters::DecoderFilterCallbacks
  uint64_t requestId() const override;
  uint64_t streamId() const override;
  const Network::Connection* connection() const override;
  void continueDecoding() override;
  SerializationType serializationType() const override;
  ProtocolType protocolType() const override;
  StreamInfo::StreamInfo& streamInfo() override;
  Router::RouteConstSharedPtr route() override;
  void sendLocalReply(const JresFilters::DirectResponse& response, bool end_stream) override;
  void startUpstreamResponse() override;
  JresFilters::UpstreamResponseStatus upstreamData(Buffer::Instance& buffer) override;
  void resetDownstreamConnection() override;
  Event::Dispatcher& dispatcher() override;
  void resetStream() override;

  void createFilterChain();
  FilterStatus applyDecoderFilters(ActiveMessageDecoderFilter* filter,
                                   FilterIterationStartState state);
  FilterStatus applyEncoderFilters(ActiveMessageEncoderFilter* filter,
                                   FilterIterationStartState state);
  void finalizeRequest();
  void onReset();
  void onError(const std::string& what);
  MessageMetadataSharedPtr metadata() const { return metadata_; }
  ContextSharedPtr context() const { return context_; }
  bool pendingStreamDecoded() const { return pending_stream_decoded_; }

private:
  void addDecoderFilterWorker(JresFilters::DecoderFilterSharedPtr filter, bool dual_filter);
  void addEncoderFilterWorker(JresFilters::EncoderFilterSharedPtr, bool dual_filter);

  ConnectionManager& parent_;

  ContextSharedPtr context_;
  MessageMetadataSharedPtr metadata_;
  Stats::TimespanPtr request_timer_;
  ActiveResponseDecoderPtr response_decoder_;

  absl::optional<Router::RouteConstSharedPtr> cached_route_;

  std::list<ActiveMessageDecoderFilterPtr> decoder_filters_;
  std::function<FilterStatus(JresFilters::DecoderFilter*)> filter_action_;

  std::list<ActiveMessageEncoderFilterPtr> encoder_filters_;
  std::function<FilterStatus(JresFilters::EncoderFilter*)> encoder_filter_action_;

  int32_t request_id_;

  // This value is used in the calculation of the weighted cluster.
  uint64_t stream_id_;
  StreamInfo::StreamInfoImpl stream_info_;

  Buffer::OwnedImpl response_buffer_;

  bool pending_stream_decoded_ : 1;
  bool local_response_sent_ : 1;

  friend class ActiveResponseDecoder;
};

using ActiveMessagePtr = std::unique_ptr<ActiveMessage>;

} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
