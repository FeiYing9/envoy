#include "extensions/filters/network/jres_proxy/router/config.h"

#include "envoy/extensions/filters/network/jres_proxy/router/v3/router.pb.h"
#include "envoy/extensions/filters/network/jres_proxy/router/v3/router.pb.validate.h"
#include "envoy/registry/registry.h"

#include "extensions/filters/network/jres_proxy/router/router_impl.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {
namespace Router {

JresFilters::FilterFactoryCb RouterFilterConfig::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::network::jres_proxy::router::v3::Router&, const std::string&,
    Server::Configuration::FactoryContext& context) {
  return [&context](JresFilters::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addDecoderFilter(std::make_shared<Router>(context.clusterManager()));
  };
}

/**
 * Static registration for the router filter. @see RegisterFactory.
 */
REGISTER_FACTORY(RouterFilterConfig, JresFilters::NamedJresFilterConfigFactory);

} // namespace Router
} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
