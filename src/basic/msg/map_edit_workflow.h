#pragma once

#include <functional>
#include <string>

namespace basic {

struct MapEditSessionResult {
  bool success{false};
  std::string message;
  std::string session_id;
  std::string navigation_map_yaml;
  std::string geometry_fingerprint;
  std::string contract_fingerprint;
};

struct MapEditPublishResult {
  bool success{false};
  std::string message;
  std::string map_id;
  std::string map_version;
  bool active{false};
};

struct MapEditCancelResult {
  bool success{false};
  std::string message;
  std::string session_id;
};

using MapEditSessionCallback = std::function<void(const MapEditSessionResult &)>;
using MapEditPublishCallback = std::function<void(const MapEditPublishResult &)>;
using MapEditCancelCallback = std::function<void(const MapEditCancelResult &)>;

}  // namespace basic
