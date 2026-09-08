#include "rclcomm.h"

#include <exception>
#include <memory>
#include <utility>

void rclcomm::StartMapEdit(const std::string &map_id,
                           const std::string &map_version,
                           const MapEditSessionCallback &callback) {
  if (!node) {
    if (callback) {
      MapEditSessionResult result;
      result.message = "ROS2 channel is not started";
      callback(result);
    }
    return;
  }
  if (!map_edit_start_client_) {
    map_edit_start_client_ =
        node->create_client<agt_robot_interfaces::srv::StartMapEdit>(
            "/agt/map/edit/start");
  }
  if (!map_edit_start_client_->service_is_ready()) {
    if (callback) {
      MapEditSessionResult result;
      result.message = "V3 Map Manager start-edit service is not ready";
      callback(result);
    }
    return;
  }

  auto request =
      std::make_shared<agt_robot_interfaces::srv::StartMapEdit::Request>();
  request->map_id = map_id;
  request->map_version = map_version;
  map_edit_start_client_->async_send_request(
      request, [callback](auto future) {
        if (!callback) {
          return;
        }
        MapEditSessionResult result;
        try {
          const auto response = future.get();
          result.success = response->success;
          result.message = response->message;
          result.session_id = response->session.session_id;
          result.navigation_map_yaml = response->session.navigation_map_yaml;
          result.geometry_fingerprint = response->session.geometry_fingerprint;
          result.contract_fingerprint = response->session.contract_fingerprint;
        } catch (const std::exception &e) {
          result.message = std::string("start map edit failed: ") + e.what();
        }
        callback(result);
      });
}

void rclcomm::PublishMapEdit(const std::string &session_id,
                             const std::string &target_map_id,
                             const std::string &target_map_version,
                             bool activate,
                             const MapEditPublishCallback &callback) {
  if (!node) {
    if (callback) {
      MapEditPublishResult result;
      result.message = "ROS2 channel is not started";
      callback(result);
    }
    return;
  }
  if (!map_edit_publish_client_) {
    map_edit_publish_client_ =
        node->create_client<agt_robot_interfaces::srv::PublishMapEdit>(
            "/agt/map/edit/publish");
  }
  if (!map_edit_publish_client_->service_is_ready()) {
    if (callback) {
      MapEditPublishResult result;
      result.message = "V3 Map Manager publish-edit service is not ready";
      callback(result);
    }
    return;
  }

  auto request =
      std::make_shared<agt_robot_interfaces::srv::PublishMapEdit::Request>();
  request->session_id = session_id;
  request->target_map_id = target_map_id;
  request->target_map_version = target_map_version;
  request->activate = activate;
  map_edit_publish_client_->async_send_request(
      request, [callback](auto future) {
        if (!callback) {
          return;
        }
        MapEditPublishResult result;
        try {
          const auto response = future.get();
          result.success = response->success;
          result.message = response->message;
          result.map_id = response->package.map_id;
          result.map_version = response->package.map_version;
          result.active = response->status.active &&
                          response->status.map_id == response->package.map_id &&
                          response->status.map_version == response->package.map_version;
        } catch (const std::exception &e) {
          result.message = std::string("publish map edit failed: ") + e.what();
        }
        callback(result);
      });
}

void rclcomm::CancelMapEdit(const std::string &session_id,
                            const MapEditCancelCallback &callback) {
  if (!node) {
    if (callback) {
      MapEditCancelResult result;
      result.session_id = session_id;
      result.message = "ROS2 channel is not started";
      callback(result);
    }
    return;
  }
  if (!map_edit_cancel_client_) {
    map_edit_cancel_client_ =
        node->create_client<agt_robot_interfaces::srv::CancelMapEdit>(
            "/agt/map/edit/cancel");
  }
  if (!map_edit_cancel_client_->service_is_ready()) {
    if (callback) {
      MapEditCancelResult result;
      result.session_id = session_id;
      result.message = "V3 Map Manager cancel-edit service is not ready";
      callback(result);
    }
    return;
  }

  auto request =
      std::make_shared<agt_robot_interfaces::srv::CancelMapEdit::Request>();
  request->session_id = session_id;
  map_edit_cancel_client_->async_send_request(
      request, [callback, session_id](auto future) {
        if (!callback) {
          return;
        }
        MapEditCancelResult result;
        result.session_id = session_id;
        try {
          const auto response = future.get();
          result.success = response->success;
          result.message = response->message;
          if (!response->session.session_id.empty()) {
            result.session_id = response->session.session_id;
          }
        } catch (const std::exception &e) {
          result.message = std::string("cancel map edit failed: ") + e.what();
        }
        callback(result);
      });
}
