#include "ros_interface.h"
#include "agt_topics.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <cmath>
#include <exception>

AgtRosInterface::AgtRosInterface(QObject *p) : QObject(p) {
  node_ = std::make_shared<rclcpp::Node>("agt_robot_hmi");
  auto qos = rclcpp::QoS(1).transient_local().reliable();
  node_->create_subscription<nav_msgs::msg::OccupancyGrid>("/map", qos,
      [this](const nav_msgs::msg::OccupancyGrid::SharedPtr m) { emit mapReceived(m); });
  node_->create_subscription<geometry_msgs::msg::PoseStamped>("/agt/robot/pose", 10,
      [this](const geometry_msgs::msg::PoseStamped::SharedPtr m) {
        const auto &q = m->pose.orientation;
        const double yaw = std::atan2(2.0 * (q.w * q.z + q.x * q.y), 1.0 - 2.0 * (q.y * q.y + q.z * q.z));
        emit poseReceived(m->pose.position.x, m->pose.position.y, yaw);
      });
  node_->create_subscription<sensor_msgs::msg::BatteryState>("/agt/robot/battery", 10,
      [this](const sensor_msgs::msg::BatteryState::SharedPtr m) { emit batteryReceived(m->percentage * 100.0); });
  node_->create_subscription<std_msgs::msg::String>("/agt/task/status", 10,
      [this](const std_msgs::msg::String::SharedPtr m) { emit taskStatusReceived(QString::fromStdString(m->data)); });
  node_->create_subscription<std_msgs::msg::String>("/agt/vision/result", 10,
      [this](const std_msgs::msg::String::SharedPtr m) {
        const auto o = QJsonDocument::fromJson(QByteArray::fromStdString(m->data)).object();
        emit visionResultReceived(o["point_id"].toString(), o["result"].toString(),
                                   o["confidence"].toDouble(), o["image_path"].toString());
      });
  task_request_ = node_->create_publisher<std_msgs::msg::String>(agt_topics::task_request, 10);
  task_start_ = node_->create_client<std_srvs::srv::Trigger>(agt_topics::task_start);
  task_pause_ = node_->create_client<std_srvs::srv::Trigger>(agt_topics::task_pause);
  task_cancel_ = node_->create_client<std_srvs::srv::Trigger>(agt_topics::task_cancel);
  navigation_goal_ = node_->create_publisher<std_msgs::msg::String>(agt_topics::navigation_go_point, 10);

  map_edit_start_ = node_->create_client<agt_robot_interfaces::srv::StartMapEdit>(
      "/agt/map/edit/start");
  map_edit_publish_ = node_->create_client<agt_robot_interfaces::srv::PublishMapEdit>(
      "/agt/map/edit/publish");
  map_edit_cancel_ = node_->create_client<agt_robot_interfaces::srv::CancelMapEdit>(
      "/agt/map/edit/cancel");

  executor_.add_node(node_);
  thread_ = std::thread([this] { spin(); });
}

AgtRosInterface::~AgtRosInterface() {
  executor_.cancel();
  if (thread_.joinable()) thread_.join();
  if (rclcpp::ok()) rclcpp::shutdown();
}

void AgtRosInterface::spin() { executor_.spin(); }

static void call(const rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr &c) {
  if (c->service_is_ready()) {
    c->async_send_request(std::make_shared<std_srvs::srv::Trigger::Request>());
  }
}

void AgtRosInterface::startTask(const QString &f) {
  std_msgs::msg::String m;
  m.data = f.toStdString();
  task_request_->publish(m);
  call(task_start_);
}

void AgtRosInterface::pauseTask() { call(task_pause_); }
void AgtRosInterface::cancelTask() { call(task_cancel_); }

void AgtRosInterface::goPoint(double x, double y, double yaw) {
  QJsonObject o;
  o["x"] = x;
  o["y"] = y;
  o["yaw"] = yaw;
  std_msgs::msg::String m;
  m.data = QJsonDocument(o).toJson(QJsonDocument::Compact).toStdString();
  navigation_goal_->publish(m);
}

void AgtRosInterface::startMapEdit(const QString &mapId, const QString &mapVersion) {
  if (!map_edit_start_ || !map_edit_start_->service_is_ready()) {
    emit mapEditSessionReceived(false, "V3 Map Manager start-edit service is not ready",
                                "", "", "", "");
    return;
  }
  auto request = std::make_shared<agt_robot_interfaces::srv::StartMapEdit::Request>();
  request->map_id = mapId.toStdString();
  request->map_version = mapVersion.toStdString();
  map_edit_start_->async_send_request(request, [this](auto future) {
    try {
      const auto response = future.get();
      emit mapEditSessionReceived(
          response->success,
          QString::fromStdString(response->message),
          QString::fromStdString(response->session.session_id),
          QString::fromStdString(response->session.navigation_map_yaml),
          QString::fromStdString(response->session.geometry_fingerprint),
          QString::fromStdString(response->session.contract_fingerprint));
    } catch (const std::exception &e) {
      emit mapEditSessionReceived(false, QString("start edit failed: %1").arg(e.what()),
                                  "", "", "", "");
    }
  });
}

void AgtRosInterface::publishMapEdit(const QString &sessionId,
                                     const QString &targetMapId,
                                     const QString &targetMapVersion,
                                     bool activate) {
  if (!map_edit_publish_ || !map_edit_publish_->service_is_ready()) {
    emit mapEditPublished(false, "V3 Map Manager publish-edit service is not ready",
                          "", "", false);
    return;
  }
  auto request = std::make_shared<agt_robot_interfaces::srv::PublishMapEdit::Request>();
  request->session_id = sessionId.toStdString();
  request->target_map_id = targetMapId.toStdString();
  request->target_map_version = targetMapVersion.toStdString();
  request->activate = activate;
  map_edit_publish_->async_send_request(request, [this](auto future) {
    try {
      const auto response = future.get();
      emit mapEditPublished(
          response->success,
          QString::fromStdString(response->message),
          QString::fromStdString(response->package.map_id),
          QString::fromStdString(response->package.map_version),
          response->status.active &&
              response->status.map_id == response->package.map_id &&
              response->status.map_version == response->package.map_version);
    } catch (const std::exception &e) {
      emit mapEditPublished(false, QString("publish edit failed: %1").arg(e.what()),
                            "", "", false);
    }
  });
}

void AgtRosInterface::cancelMapEdit(const QString &sessionId) {
  if (!map_edit_cancel_ || !map_edit_cancel_->service_is_ready()) {
    emit mapEditCancelled(false, "V3 Map Manager cancel-edit service is not ready",
                          sessionId);
    return;
  }
  auto request = std::make_shared<agt_robot_interfaces::srv::CancelMapEdit::Request>();
  request->session_id = sessionId.toStdString();
  map_edit_cancel_->async_send_request(request, [this, sessionId](auto future) {
    try {
      const auto response = future.get();
      const QString returned_id = response->session.session_id.empty()
          ? sessionId
          : QString::fromStdString(response->session.session_id);
      emit mapEditCancelled(response->success,
                            QString::fromStdString(response->message), returned_id);
    } catch (const std::exception &e) {
      emit mapEditCancelled(false, QString("cancel edit failed: %1").arg(e.what()),
                            sessionId);
    }
  });
}
