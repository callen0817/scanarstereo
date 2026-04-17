cmake_minimum_required(VERSION 3.8)
project(scanar_gui)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

# Install launch files
install(DIRECTORY launch config scripts
  DESTINATION share/${PROJECT_NAME}
)

# Install Python scripts
install(PROGRAMS scripts/data_quality_agent.py
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
