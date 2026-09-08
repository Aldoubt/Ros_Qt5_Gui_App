# AGT Robot HMI architecture

The HMI is a ROS 2 Humble presentation and editing client. Runtime navigation,
localization, map lifecycle, camera/gimbal drivers and AI models remain outside
this repository.

## V3 boundary

For `agt_navigation_v3` integration the ownership rule is:

```text
agt_navigation_v3 / agt_map_manager
  owns map identity, versions, active-map state, MapEditSession lifecycle,
  geometry validation, localization PCD, relocalization assets and RTK origin
                |
          stable ROS/runtime contract
                |
agt_robot_hmi
  owns display, operator interaction, occupancy painting and topology editing
```

The HMI must never overwrite an immutable released Map Package or maintain a
second local source of truth for `active map`.

Navigation mode consumes the runtime configuration prepared from V3's selected
`active_map.yaml`. Edit mode consumes the staging `map.yaml` created by a V3
MapEditSession. A published edit becomes a new package/version only after V3
validates it.

## Map coordinate contract

New V3 integration code uses `src/basic/map/map_coordinate_transform.h` rather
than introducing widget-local coordinate formulas.

V1 supports axis-aligned Nav2 map origins only (`origin[2] == 0`). The same rule
is enforced by V3 at publication.

Coordinate definitions:

```text
scene: +x right, +y down, continuous image coordinates
grid:  +x right, +y up, integer OccupancyGrid cell indices
world: metres in frame map
```

World to grid uses floor, not rounding before division:

```text
column = floor((world_x - origin_x) / resolution)
row    = floor((world_y - origin_y) / resolution)
```

Cell corner and cell centre are separate explicit operations. Existing Qt scene
transforms are migrated incrementally because they represent continuous map
coordinates; silently adding `+0.5 cell` to those legacy overlays would shift
robot/topology rendering.

When `BUILD_WITH_TEST=ON`, `map_coordinate_transform_test` checks indexing,
non-zero origin round trips, cell-centre semantics, boundaries and the V1 yaw
constraint.

## Existing task/vision transport

The current task/vision transport still uses placeholder contracts while the
runtime-facing task interfaces are finalized:

| Purpose | ROS 2 endpoint | Placeholder type |
|---|---|---|
| task file handoff | `/agt/task/request` | `std_msgs/msg/String` |
| start/pause/cancel | `/agt/task/{start,pause,cancel}` | `std_srvs/srv/Trigger` |
| task status | `/agt/task/status` | `std_msgs/msg/String` |
| vision result | `/agt/vision/result` | JSON in `std_msgs/msg/String` |

Those placeholders live behind `src/ros_interface` and are independent of the
V3 map ownership contract above.
