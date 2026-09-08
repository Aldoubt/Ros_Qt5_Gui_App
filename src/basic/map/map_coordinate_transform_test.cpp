#include "map_coordinate_transform.h"

#include <gtest/gtest.h>

namespace basic {
namespace {

TEST(MapCoordinateTransformTest, UsesFloorForWorldToGrid) {
  MapCoordinateTransform tf(0.0, 0.0, 0.1, 20, 20);
  int column = -1;
  int row = -1;
  EXPECT_TRUE(tf.WorldToGrid(0.46, 0.24, column, row));
  EXPECT_EQ(column, 4);
  EXPECT_EQ(row, 2);
}

TEST(MapCoordinateTransformTest, PreservesNonZeroOriginThroughSceneRoundTrip) {
  MapCoordinateTransform tf(-47.36, 82.19, 0.05, 400, 300);
  double scene_x = 0.0;
  double scene_y = 0.0;
  tf.WorldToScene(-44.11, 87.34, scene_x, scene_y);

  double world_x = 0.0;
  double world_y = 0.0;
  tf.SceneToWorld(scene_x, scene_y, world_x, world_y);
  EXPECT_NEAR(world_x, -44.11, 1e-12);
  EXPECT_NEAR(world_y, 87.34, 1e-12);
}

TEST(MapCoordinateTransformTest, CellCenterIsExplicit) {
  MapCoordinateTransform tf(-1.0, 2.0, 0.1, 10, 10);
  double world_x = 0.0;
  double world_y = 0.0;
  tf.GridCellCenterToWorld(3, 4, world_x, world_y);
  EXPECT_NEAR(world_x, -0.65, 1e-12);
  EXPECT_NEAR(world_y, 2.45, 1e-12);
}

TEST(MapCoordinateTransformTest, RejectsOutOfBoundsGridLookup) {
  MapCoordinateTransform tf(0.0, 0.0, 1.0, 3, 2);
  int column = -1;
  int row = -1;
  EXPECT_FALSE(tf.WorldToGrid(3.0, 0.5, column, row));
  EXPECT_EQ(column, 3);
  EXPECT_EQ(row, 0);
}

TEST(MapCoordinateTransformTest, V1EditContractRequiresAxisAlignedYaw) {
  EXPECT_TRUE(MapCoordinateTransform::IsAxisAlignedYaw(0.0));
  EXPECT_TRUE(MapCoordinateTransform::IsAxisAlignedYaw(1e-13));
  EXPECT_FALSE(MapCoordinateTransform::IsAxisAlignedYaw(1e-4));
}

}  // namespace
}  // namespace basic
