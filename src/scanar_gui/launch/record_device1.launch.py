import os
from launch import LaunchDescription
from launch.actions import ExecuteProcess, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    ns = 'dev1'
    
    # We bring up the sensors using the existing device1_master.launch.py
    # but since fast_livo is commented out, it won't pollute the bag
    bringup = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(get_package_share_directory('scanar_gui'), 'launch', 'device1_master.launch.py')
        )
    )

    # The Data Quality Agent checks will be handled by post-processing or 
    # monitoring scripts. We record everything we need.
    record = ExecuteProcess(
        cmd=['ros2', 'bag', 'record', '-o', '/home/scanarst/scanarv2/calibration/device 1/dev1_calibration_bag',
             f'/{ns}/rslidar_points', 
             f'/{ns}/imu/data', 
             f'/{ns}/oak/rgb/image_raw',
             f'/{ns}/oak/rgb/camera_info'],
        output='screen'
    )

    return LaunchDescription([
        bringup,
        record
    ])
