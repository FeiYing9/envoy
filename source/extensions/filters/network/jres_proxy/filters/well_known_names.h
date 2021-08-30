#pragma once

#include <string>

#include "common/singleton/const_singleton.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {
namespace JresFilters {

/**
 * Well-known Jres filter names.
 * NOTE: New filters should use the well known name: envoy.filters.jres.name.
 */
class JresFilterNameValues {
public:
  // Router filter
  const std::string ROUTER = "envoy.filters.jres.router";
};

using JresFilterNames = ConstSingleton<JresFilterNameValues>;

} // namespace JresFilters
} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
