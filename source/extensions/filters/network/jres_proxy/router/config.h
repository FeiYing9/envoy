#pragma once

#include "envoy/extensions/filters/network/jres_proxy/router/v3/router.pb.h"
#include "envoy/extensions/filters/network/jres_proxy/router/v3/router.pb.validate.h"

#include "extensions/filters/network/jres_proxy/filters/factory_base.h"
#include "extensions/filters/network/jres_proxy/filters/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {
namespace Router {

class RouterFilterConfig
    : public JresFilters::FactoryBase<
          envoy::extensions::filters::network::jres_proxy::router::v3::Router> {
public:
  RouterFilterConfig() : FactoryBase(JresFilters::JresFilterNames::get().ROUTER) {}

private:
  JresFilters::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::network::jres_proxy::router::v3::Router& proto_config,
      const std::string& stat_prefix, Server::Configuration::FactoryContext& context) override;
};

} // namespace Router
} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
