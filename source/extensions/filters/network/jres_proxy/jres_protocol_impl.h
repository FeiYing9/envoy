#pragma once

#include "extensions/filters/network/jres_proxy/protocol.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {

class JresProtocolImpl : public Protocol {
public:
  JresProtocolImpl() = default;
  ~JresProtocolImpl() override = default;

  const std::string& name() const override { return ProtocolNames::get().fromType(type()); }
  ProtocolType type() const override { return ProtocolType::Jres; }

  std::pair<ContextSharedPtr, bool> decodeHeader(Buffer::Instance& buffer,
                                                 MessageMetadataSharedPtr metadata) override;
  bool decodeData(Buffer::Instance& buffer, ContextSharedPtr context,
                  MessageMetadataSharedPtr metadata) override;

  bool encode(Buffer::Instance& buffer, const MessageMetadata& metadata, const std::string& content,
              RpcResponseType type) override;

  static constexpr uint8_t MessageSize = 16;
  static constexpr int32_t MaxBodySize = 16 * 1024 * 1024;
};

} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
