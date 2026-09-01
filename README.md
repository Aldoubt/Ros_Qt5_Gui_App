# AGT Robot HMI

ROS2 Humble based inspection robot operator console for AGT Robotics.

The HMI handles map/status display, operator interaction and inspection-task
editing. Robot business logic remains in `agt_navigation_runtime` and
`agt_vision_runtime`; the Qt process does not directly control Nav2, drivers,
gimbals, or AI models.

Inspection points are stored in `config/tasks/*.yaml`. The current ROS boundary
uses standard-message placeholders; see [`docs/architecture.md`](docs/architecture.md)
for the contract and migration path to `agt_robot_hmi_interfaces`.

## Build

```bash
source /opt/ros/humble/setup.bash
colcon build --packages-select agt_robot_hmi
source install/setup.bash
ros2 run agt_robot_hmi agt_robot_hmi
```

The application displays `/map`, `/agt/robot/pose`, and `/agt/robot/battery`,
and accepts task status and JSON vision results from the AGT runtime/mock.
Right-click the map to add an inspection point or set a navigation goal.
