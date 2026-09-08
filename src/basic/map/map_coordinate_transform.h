#ifndef AGT_MAP_COORDINATE_TRANSFORM_H
#define AGT_MAP_COORDINATE_TRANSFORM_H

#include <cmath>

namespace basic {

// Canonical axis-aligned Nav2 map geometry used by the V3 HMI integration.
//
// Scene coordinates follow the loaded map image: +x right, +y down.
// Grid coordinates follow nav_msgs/OccupancyGrid: +x/right, +y/up.
// World coordinates are metres in frame "map".
//
// V1 deliberately supports only origin yaw == 0.0.  V3 Map Manager enforces
// the same contract at edit-session publication, so the HMI must not invent a
// second rotated-map transform implementation.
class MapCoordinateTransform {
 public:
  MapCoordinateTransform(double origin_x, double origin_y, double resolution,
                         int width, int height)
      : origin_x_(origin_x),
        origin_y_(origin_y),
        resolution_(resolution),
        width_(width),
        height_(height) {}

  static bool IsAxisAlignedYaw(double yaw, double tolerance = 1e-12) {
    return std::isfinite(yaw) && std::abs(yaw) <= tolerance;
  }

  bool IsValid() const {
    return std::isfinite(origin_x_) && std::isfinite(origin_y_) &&
           std::isfinite(resolution_) && resolution_ > 0.0 && width_ > 0 &&
           height_ > 0;
  }

  bool WorldToGrid(double world_x, double world_y, int &column,
                   int &row) const {
    if (!IsValid() || !std::isfinite(world_x) || !std::isfinite(world_y)) {
      return false;
    }
    column = static_cast<int>(std::floor((world_x - origin_x_) / resolution_));
    row = static_cast<int>(std::floor((world_y - origin_y_) / resolution_));
    return column >= 0 && row >= 0 && column < width_ && row < height_;
  }

  void GridCornerToWorld(int column, int row, double &world_x,
                         double &world_y) const {
    world_x = origin_x_ + static_cast<double>(column) * resolution_;
    world_y = origin_y_ + static_cast<double>(row) * resolution_;
  }

  void GridCellCenterToWorld(int column, int row, double &world_x,
                             double &world_y) const {
    world_x = origin_x_ + (static_cast<double>(column) + 0.5) * resolution_;
    world_y = origin_y_ + (static_cast<double>(row) + 0.5) * resolution_;
  }

  void WorldToScene(double world_x, double world_y, double &scene_x,
                    double &scene_y) const {
    scene_x = (world_x - origin_x_) / resolution_;
    scene_y = static_cast<double>(height_) -
              (world_y - origin_y_) / resolution_;
  }

  void SceneToWorld(double scene_x, double scene_y, double &world_x,
                    double &world_y) const {
    world_x = scene_x * resolution_ + origin_x_;
    world_y = (static_cast<double>(height_) - scene_y) * resolution_ + origin_y_;
  }

  bool GridInBounds(int column, int row) const {
    return column >= 0 && row >= 0 && column < width_ && row < height_;
  }

 private:
  double origin_x_{0.0};
  double origin_y_{0.0};
  double resolution_{0.0};
  int width_{0};
  int height_{0};
};

}  // namespace basic

#endif  // AGT_MAP_COORDINATE_TRANSFORM_H
