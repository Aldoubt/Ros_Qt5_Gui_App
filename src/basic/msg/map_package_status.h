#pragma once

#include <cstdint>
#include <string>

namespace basic {

// Transport-neutral snapshot of V3 Map Manager status. The UI uses this exact
// identity as edit context instead of selecting an arbitrary local map path.
struct MapPackageStatus {
  bool active{false};
  std::uint64_t generation{0};
  std::string map_id;
  std::string map_version;
  std::string package_path;
  std::string navigation_map_yaml;
  std::string localization_map_pcd;
  std::string relocalization_assets_path;
  std::string rtk_origin_yaml;
  std::string reason;

  bool HasExactActiveMap() const {
    return active && !map_id.empty() && !map_version.empty();
  }
};

}  // namespace basic
