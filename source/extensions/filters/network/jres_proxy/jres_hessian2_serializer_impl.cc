#include "extensions/filters/network/jres_proxy/jres_hessian2_serializer_impl.h"

#include "envoy/common/exception.h"

#include "common/common/assert.h"
#include "common/common/macros.h"

#include "extensions/filters/network/jres_proxy/hessian_utils.h"
#include "extensions/filters/network/jres_proxy/message_impl.h"
#include "extensions/filters/network/jres_proxy/serializer.h"
#include "extensions/filters/network/jres_proxy/serializer_impl.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {

std::pair<RpcInvocationSharedPtr, bool>
JresHessian2SerializerImpl::deserializeRpcInvocation(Buffer::Instance& buffer,
                                                      ContextSharedPtr context) {
  size_t total_size = 0, size;
  // TODO(zyfjeff): Add format checker
  std::string jres_version = HessianUtils::peekString(buffer, &size);
  total_size += size;
  std::string service_name = HessianUtils::peekString(buffer, &size, total_size);
  total_size += size;
  std::string service_version = HessianUtils::peekString(buffer, &size, total_size);
  total_size += size;
  std::string method_name = HessianUtils::peekString(buffer, &size, total_size);
  total_size += size;

  if (static_cast<uint64_t>(context->bodySize()) < total_size) {
    throw EnvoyException(fmt::format("RpcInvocation size({}) large than body size({})", total_size,
                                     context->bodySize()));
  }

  auto invo = std::make_shared<RpcInvocationImpl>();
  invo->setServiceName(service_name);
  invo->setServiceVersion(service_version);
  invo->setMethodName(method_name);

  return std::pair<RpcInvocationSharedPtr, bool>(invo, true);
}

std::pair<RpcResultSharedPtr, bool>
JresHessian2SerializerImpl::deserializeRpcResult(Buffer::Instance& buffer,
                                                  ContextSharedPtr context) {
  ASSERT(buffer.length() >= context->bodySize());
  size_t total_size = 0;
  bool has_value = true;

  auto result = std::make_shared<RpcResultImpl>();
  RpcResponseType type = static_cast<RpcResponseType>(HessianUtils::peekInt(buffer, &total_size));

  switch (type) {
  case RpcResponseType::ResponseWithException:
  case RpcResponseType::ResponseWithExceptionWithAttachments:
    result->setException(true);
    break;
  case RpcResponseType::ResponseWithNullValue:
  case RpcResponseType::ResponseNullValueWithAttachments:
    has_value = false;
    FALLTHRU;
  case RpcResponseType::ResponseWithValue:
  case RpcResponseType::ResponseValueWithAttachments:
    result->setException(false);
    break;
  default:
    throw EnvoyException(fmt::format("not supported return type {}", static_cast<uint8_t>(type)));
  }

  if (context->bodySize() < total_size) {
    throw EnvoyException(fmt::format("RpcResult size({}) large than body size({})", total_size,
                                     context->bodySize()));
  }

  if (!has_value && context->bodySize() != total_size) {
    throw EnvoyException(
        fmt::format("RpcResult is no value, but the rest of the body size({}) not equal 0",
                    (context->bodySize() - total_size)));
  }

  return std::pair<RpcResultSharedPtr, bool>(result, true);
}

size_t JresHessian2SerializerImpl::serializeRpcResult(Buffer::Instance& output_buffer,
                                                       const std::string& content,
                                                       RpcResponseType type) {
  size_t origin_length = output_buffer.length();

  // The serialized response type is compact int.
  size_t serialized_size = HessianUtils::writeInt(
      output_buffer, static_cast<std::underlying_type<RpcResponseType>::type>(type));

  // Serialized response content.
  serialized_size += HessianUtils::writeString(output_buffer, content);

  ASSERT((output_buffer.length() - origin_length) == serialized_size);

  return serialized_size;
}

class JresHessian2SerializerConfigFactory
    : public SerializerFactoryBase<JresHessian2SerializerImpl> {
public:
  JresHessian2SerializerConfigFactory()
      : SerializerFactoryBase(ProtocolType::Jres, SerializationType::Hessian2) {}
};

/**
 * Static registration for the Hessian protocol. @see RegisterFactory.
 */
REGISTER_FACTORY(JresHessian2SerializerConfigFactory, NamedSerializerConfigFactory);

} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
