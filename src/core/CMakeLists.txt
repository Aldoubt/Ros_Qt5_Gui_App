set(AGT_CORE_SOURCES
  core_main.cpp
  mainwindow.cpp
  map_widget.cpp
)
set(AGT_CORE_HEADERS
  mainwindow.h
  map_widget.h
)
add_executable(agt_robot_hmi ${AGT_CORE_SOURCES} ${AGT_CORE_HEADERS})
qt5_wrap_cpp(AGT_CORE_MOC mainwindow.h map_widget.h)
target_sources(agt_robot_hmi PRIVATE ${AGT_CORE_MOC})
target_include_directories(agt_robot_hmi PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(agt_robot_hmi agt_ros_interface agt_inspection Qt5::Core Qt5::Widgets yaml-cpp)
ament_target_dependencies(agt_robot_hmi rclcpp geometry_msgs nav_msgs sensor_msgs std_msgs std_srvs tf2 tf2_geometry_msgs cv_bridge)
set_target_properties(agt_robot_hmi PROPERTIES AUTOMOC OFF AUTOUIC ON AUTORCC ON)
install(TARGETS agt_robot_hmi RUNTIME DESTINATION bin)
