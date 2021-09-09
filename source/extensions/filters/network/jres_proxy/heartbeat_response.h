#pragma once

#include "extensions/filters/network/jres_proxy/filters/filter.h"
#include "extensions/filters/network/jres_proxy/metadata.h"
#include "extensions/filters/network/jres_proxy/protocol.h"
#include "extensions/filters/network/jres_proxy/serializer.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace JresProxy {

struct HeartbeatResponse : public JresFilters::DirectResponse, Logger::Loggable<Logger::Id::jres> {
  HeartbeatResponse() = default;
  ~HeartbeatResponse() override = default;

  using ResponseType = JresFilters::DirectResponse::ResponseType;
  ResponseType encode(MessageMetadata& metadata, Protocol& protocol,
                      Buffer::Instance& buffer) const override;
};

} // namespace JresProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
