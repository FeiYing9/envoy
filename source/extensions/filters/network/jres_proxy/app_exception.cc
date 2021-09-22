#include "extensions/filters/network/jres_proxy/app_exception.h"

#include "common/buffer/buffer_impl.h"

#include "extensions/filters/network/jres_proxy/message.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {

DownstreamConnectionCloseException::DownstreamConnectionCloseException(const std::string& what)
    : EnvoyException(what) {}

} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
