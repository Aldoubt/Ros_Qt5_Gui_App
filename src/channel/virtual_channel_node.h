/*
 * @Author: chengyang chengyangkj@outlook.com
 * @Date: 2023-07-25 16:20:39
 * @LastEditors: chengyangkj chengyangkj@qq.com
 * @LastEditTime: 2023-10-07 14:41:45
 * @FilePath:
 * /ros_qt5_gui_app/include/channel/base/virtual_communcation_node.h
 */
#pragma once
#include <any>
#include <atomic>
#include <thread>
#include "msg/msg_info.h"
#include "msg/map_edit_workflow.h"
#include "msg/map_package_status.h"
#include "occupancy_map.h"
#include "topology_map.h"
#include "point_type.h"
#include "core/framework/framework.h"
using namespace basic;
class VirtualChannelNode {
 private:
  std::thread process_thread_;

 public:
  VirtualChannelNode(/* args */) {}
  
  bool Init() {
    if (Start()) {
      std::cout << "start channel success" << std::endl;
      run_flag_ = true;
      process_thread_ = std::thread([this]() {
        while (run_flag_) {
          Process();
          std::this_thread::sleep_for(
              std::chrono::milliseconds(1000 / loop_rate_));
        }
      });
      return true;
    }
    return false;
  }
  void ShutDown() {
    run_flag_ = false;
    if (process_thread_.joinable()) {
      process_thread_.join();
    }
    Stop();
  }
  virtual ~VirtualChannelNode() {}
  virtual void Process() {}
  virtual bool Start() = 0;
  virtual bool Stop() = 0;
  virtual std::string Name() = 0;
  
  virtual bool IsConnecting() const { return false; }
  virtual bool IsConnectionFailed() const { return false; }
  virtual std::string GetConnectionError() const { return ""; }

  // Exact active-map context supplied by the system-of-record. Unsupported
  // channels return an empty snapshot; they never infer identity from a file.
  virtual MapPackageStatus GetMapPackageStatus() const {
    return MapPackageStatus{};
  }

  // Transport-neutral V3 map workflow boundary. Non-V3 channels remain usable
  // and report an explicit unsupported result instead of exposing ROS service
  // types to MainWindow/SceneManager.
  virtual void StartMapEdit(const std::string &, const std::string &,
                            const MapEditSessionCallback &callback) {
    if (callback) {
      MapEditSessionResult result;
      result.message = "map edit workflow is unsupported by this channel";
      callback(result);
    }
  }
  virtual void PublishMapEdit(const std::string &, const std::string &,
                              const std::string &, bool,
                              const MapEditPublishCallback &callback) {
    if (callback) {
      MapEditPublishResult result;
      result.message = "map edit workflow is unsupported by this channel";
      callback(result);
    }
  }
  virtual void CancelMapEdit(const std::string &session_id,
                             const MapEditCancelCallback &callback) {
    if (callback) {
      MapEditCancelResult result;
      result.session_id = session_id;
      result.message = "map edit workflow is unsupported by this channel";
      callback(result);
    }
  }

 public:
  int loop_rate_{30};
  std::atomic<bool> run_flag_{false};
};
