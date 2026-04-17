import os
from launch import LaunchDescription
from launch.actions import ExecuteProcess, IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node

def generate_launch_description():
    ns = 'dev1'
    
    # 1. Bring up all hardware sensors
    bringup = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(get_package_share_directory('scanar_gui'), 'launch', 'device1_master.launch.py')
        )
    )

    # 2. Start the Data Quality Agent to enforce >=200Hz IMU and >=25FPS Camera
    qc_agent = Node(
        package='scanar_gui',
        executable='data_quality_agent.py',
        name='data_quality_agent',
        output='screen'
    )

    # 3. Start recording the rosbag after 5 seconds to let streams stabilize
    record = TimerAction(
        period=5.0,
        actions=[
            ExecuteProcess(
                cmd=['ros2', 'bag', 'record', '-o', '/home/scanarst/scanarv2/calibration/device 1/dev1_calibration_bag',
                     f'/{ns}/rslidar_points', 
                     f'/{ns}/imu/data', 
                     f'/{ns}/oak/rgb/image_raw',
                     f'/{ns}/oak/rgb/camera_info'],
                output='screen'
            )
        ]
    )

    return LaunchDescription([
        bringup,
        qc_agent,
        record
    ])
