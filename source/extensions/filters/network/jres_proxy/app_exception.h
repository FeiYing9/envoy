#pragma once

#include "envoy/common/exception.h"

#include "common/common/utility.h"

#include "extensions/filters/network/jres_proxy/filters/filter.h"
#include "extensions/filters/network/jres_proxy/metadata.h"
#include "extensions/filters/network/jres_proxy/protocol.h"
#include "extensions/filters/network/jres_proxy/serializer.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {

using ResponseType = JresFilters::DirectResponse::ResponseType;

template <typename T = ResponseStatus>
struct AppExceptionBase : public EnvoyException,
                          public JresFilters::DirectResponse,
                          Logger::Loggable<Logger::Id::jres> {
  AppExceptionBase(const AppExceptionBase& ex) = default;
  AppExceptionBase(T status, const std::string& what)
      : EnvoyException(what), status_(status),
        response_type_(RpcResponseType::ResponseWithException) {}

  ResponseType encode(MessageMetadata& metadata, JresProxy::Protocol& protocol,
                      Buffer::Instance& buffer) const override {
    ASSERT(buffer.length() == 0);

    ENVOY_LOG(debug, "Exception information: {}", what());

    metadata.setResponseStatus<T>(status_);
    metadata.setMessageType(MessageType::Response);
    if (!protocol.encode(buffer, metadata, what(), response_type_)) {
      ExceptionUtil::throwEnvoyException("Failed to encode local reply message");
    }

    return ResponseType::Exception;
  }

  const T status_;
  const RpcResponseType response_type_;
};

using AppException = AppExceptionBase<>;

struct DownstreamConnectionCloseException : public EnvoyException {
  DownstreamConnectionCloseException(const std::string& what);
};

} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
