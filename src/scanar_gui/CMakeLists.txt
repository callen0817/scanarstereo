cmake_minimum_required(VERSION 3.5)
project(scanar_gui)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(OpenCV REQUIRED)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)

# Explicitly list headers for AUTOMOC
set(GUI_HEADERS
  include/scanar_gui/mainwindow.h
  include/scanar_gui/ros_worker.h
)

add_executable(scanar_gui_node
  src/main.cpp
  src/mainwindow.cpp
  src/ros_worker.cpp
  ${GUI_HEADERS}
)

target_include_directories(scanar_gui_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries(scanar_gui_node
  Qt5::Widgets
  Qt5::Core
  Qt5::Gui
  ${rclcpp_LIBRARIES}
  ${sensor_msgs_LIBRARIES}
  ${nav_msgs_LIBRARIES}
  ${geometry_msgs_LIBRARIES}
  ${cv_bridge_LIBRARIES}
  ${image_transport_LIBRARIES}
  ${OpenCV_LIBRARIES}
)

install(TARGETS scanar_gui_node
  DESTINATION lib/${PROJECT_NAME})

# Use standard ament macro
ament_target_dependencies(scanar_gui_node
  rclcpp
  sensor_msgs
  nav_msgs
  geometry_msgs
  cv_bridge
  image_transport
)

ament_package()
