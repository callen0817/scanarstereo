cmake_minimum_required(VERSION 3.14)
project(uins3_driver)

add_compile_options(-w -O3)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

include(FetchContent)
FetchContent_Declare(
  inertialsensesdk
  GIT_REPOSITORY https://github.com/inertialsense/inertial-sense-sdk.git
  GIT_TAG 1.11.1
)
FetchContent_MakeAvailable(inertialsensesdk)

set(CMAKE_CXX_STANDARD 17)

add_executable(uins3_node src/uins3_node.cpp)
target_include_directories(uins3_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${inertialsensesdk_SOURCE_DIR}/src>
  /usr/include/libusb-1.0
)

# The SDK's CMakeLists uses its project name for the static library target
target_link_libraries(uins3_node
  InertialSense
  ${rclcpp_LIBRARIES}
  ${sensor_msgs_LIBRARIES}
  ${nav_msgs_LIBRARIES}
  ${tf2_ros_LIBRARIES}
  ${tf2_geometry_msgs_LIBRARIES}
)

install(TARGETS uins3_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME})

ament_target_dependencies(uins3_node rclcpp sensor_msgs nav_msgs tf2_ros tf2_geometry_msgs)
ament_package()
