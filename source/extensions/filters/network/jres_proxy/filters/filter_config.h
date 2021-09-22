#pragma once

#include <string>

#include "envoy/common/pure.h"
#include "envoy/config/typed_config.h"
#include "envoy/server/filter_config.h"

#include "common/common/macros.h"
#include "common/protobuf/protobuf.h"

#include "extensions/filters/network/jres_proxy/filters/filter.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {
namespace JresFilters {

/**
 * Implemented by each Jres filter and registered via Registry::registerFactory or the
 * convenience class RegisterFactory.
 */
class NamedJresFilterConfigFactory : public Envoy::Config::TypedFactory {
public:
  ~NamedJresFilterConfigFactory() override = default;

  /**
   * Create a particular Jres filter factory implementation. If the implementation is unable to
   * produce a factory with the provided parameters, it should throw an EnvoyException in the case
   * of general error. The returned callback should always be initialized.
   * @param config supplies the configuration for the filter
   * @param stat_prefix prefix for stat logging
   * @param context supplies the filter's context.
   * @return FilterFactoryCb the factory creation function.
   */
  virtual JresFilters::FilterFactoryCb
  createFilterFactoryFromProto(const Protobuf::Message& config, const std::string& stat_prefix,
                               Server::Configuration::FactoryContext& context) PURE;

  std::string category() const override { return "envoy.jres_proxy.filters"; }
};

} // namespace JresFilters
} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
