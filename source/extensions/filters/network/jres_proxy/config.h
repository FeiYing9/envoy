#pragma once

#include <string>

#include "envoy/extensions/filters/network/jres_proxy/v3/jres_proxy.pb.h"
#include "envoy/extensions/filters/network/jres_proxy/v3/jres_proxy.pb.validate.h"

#include "extensions/filters/network/common/factory_base.h"
#include "extensions/filters/network/jres_proxy/conn_manager.h"
#include "extensions/filters/network/jres_proxy/filters/filter.h"
#include "extensions/filters/network/jres_proxy/router/route_matcher.h"
#include "extensions/filters/network/jres_proxy/router/router_impl.h"
#include "extensions/filters/network/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {

/**
 * Config registration for the jres proxy filter. @see NamedNetworkFilterConfigFactory.
 */
class JresProxyFilterConfigFactory
    : public Common::FactoryBase<envoy::extensions::filters::network::jres_proxy::v3::JresProxy> {
public:
  JresProxyFilterConfigFactory() : FactoryBase(NetworkFilterNames::get().JresProxy, true) {}

private:
  Network::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::network::jres_proxy::v3::JresProxy& proto_config,
      Server::Configuration::FactoryContext& context) override;
};

class ConfigImpl : public Config,
                   public Router::Config,
                   public JresFilters::FilterChainFactory,
                   Logger::Loggable<Logger::Id::config> {
public:
  using JresProxyConfig = envoy::extensions::filters::network::jres_proxy::v3::JresProxy;
  using JresFilterConfig = envoy::extensions::filters::network::jres_proxy::v3::JresFilter;

  ConfigImpl(const JresProxyConfig& config, Server::Configuration::FactoryContext& context);
  ~ConfigImpl() override = default;

  // JresFilters::FilterChainFactory
  void createFilterChain(JresFilters::FilterChainFactoryCallbacks& callbacks) override;

  // Router::Config
  Router::RouteConstSharedPtr route(const MessageMetadata& metadata,
                                    uint64_t random_value) const override;

  // Config
  JresFilterStats& stats() override { return stats_; }
  JresFilters::FilterChainFactory& filterFactory() override { return *this; }
  Router::Config& routerConfig() override { return *this; }
  ProtocolPtr createProtocol() override;

private:
  void registerFilter(const JresFilterConfig& proto_config);

  Server::Configuration::FactoryContext& context_;
  const std::string stats_prefix_;
  JresFilterStats stats_;
  const SerializationType serialization_type_;
  const ProtocolType protocol_type_;
  Router::RouteMatcherPtr route_matcher_;

  std::list<JresFilters::FilterFactoryCb> filter_factories_;
};

} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
