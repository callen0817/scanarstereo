#!/bin/bash
source /opt/ros/humble/setup.bash
source install/setup.bash
ros2 launch scanar_gui record_device1_with_qc.launch.py
