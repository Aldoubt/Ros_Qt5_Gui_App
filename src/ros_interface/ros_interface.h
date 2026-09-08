#pragma once
#include <QObject>
#include <QString>
#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <sensor_msgs/msg/battery_state.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_srvs/srv/trigger.hpp>
#include <agt_robot_interfaces/srv/start_map_edit.hpp>
#include <agt_robot_interfaces/srv/publish_map_edit.hpp>
#include <agt_robot_interfaces/srv/cancel_map_edit.hpp>

class AgtRosInterface : public QObject {
  Q_OBJECT
 public:
  explicit AgtRosInterface(QObject *parent=nullptr);
  ~AgtRosInterface();

  void startTask(const QString &taskFile);
  void pauseTask();
  void cancelTask();
  void goPoint(double x,double y,double yaw);

  // V3 Map Manager is the authority. These calls never read/write released
  // package files directly from the HMI process.
  void startMapEdit(const QString &mapId, const QString &mapVersion);
  void publishMapEdit(const QString &sessionId, const QString &targetMapId,
                      const QString &targetMapVersion, bool activate);
  void cancelMapEdit(const QString &sessionId);

 signals:
  void mapReceived(const nav_msgs::msg::OccupancyGrid::SharedPtr);
  void poseReceived(double x,double y,double yaw);
  void batteryReceived(double percent);
  void laserReceived(const sensor_msgs::msg::LaserScan::SharedPtr);
  void imageReceived(const sensor_msgs::msg::Image::SharedPtr);
  void taskStatusReceived(const QString&);
  void visionResultReceived(const QString&,const QString&,double,const QString&);

  void mapEditSessionReceived(bool success, const QString &message,
                              const QString &sessionId,
                              const QString &navigationMapYaml,
                              const QString &geometryFingerprint,
                              const QString &contractFingerprint);
  void mapEditPublished(bool success, const QString &message,
                        const QString &mapId, const QString &mapVersion,
                        bool active);
  void mapEditCancelled(bool success, const QString &message,
                        const QString &sessionId);

 private:
  void spin();
  rclcpp::Node::SharedPtr node_;
  rclcpp::executors::SingleThreadedExecutor executor_;
  std::thread thread_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr task_request_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr task_start_,task_pause_,task_cancel_;
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr navigation_goal_;
  rclcpp::Client<agt_robot_interfaces::srv::StartMapEdit>::SharedPtr map_edit_start_;
  rclcpp::Client<agt_robot_interfaces::srv::PublishMapEdit>::SharedPtr map_edit_publish_;
  rclcpp::Client<agt_robot_interfaces::srv::CancelMapEdit>::SharedPtr map_edit_cancel_;
};
