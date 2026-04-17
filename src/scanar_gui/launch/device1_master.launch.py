import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, GroupAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node, PushRosNamespace

def generate_launch_description():
    ns = 'dev1'

    uins3_launch_dir = os.path.join(get_package_share_directory('uins3_driver'), 'launch')
    depthai_launch_dir = os.path.join(get_package_share_directory('depthai_ros_driver'), 'launch')
    
    # In ROS 2, nested IncludeLaunchDescriptions sometimes don't automatically inherit the 
    # GroupAction PushRosNamespace unless explicitly passed or properly scoped.
    # To enforce the namespace globally, we pass it down or explicitly remap.
    
    uins3_driver = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(uins3_launch_dir, 'uins3.launch.py'))
    )

    rslidar_config = '/home/scanarst/scanarv2/scanarstereo/src/rslidar_sdk/config/config.yaml'
    rslidar_driver = Node(
        package='rslidar_sdk',
        executable='rslidar_sdk_node',
        name='rslidar_sdk_node',
        output='screen',
        parameters=[{'config_path': rslidar_config}],
        remappings=[
            ('/rslidar_points', f'/{ns}/rslidar_points'),
            ('/rslidar_packets', f'/{ns}/rslidar_packets')
        ]
    )

    depthai_config = os.path.join(get_package_share_directory('scanar_gui'), 'config', 'oak_calibration.yaml')
    depthai_driver = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(depthai_launch_dir, 'camera.launch.py')),
        launch_arguments={
            'name': 'oak', 
            'parent_frame': ns + '/base_link',
            'params_file': depthai_config
        }.items()
    )

    device1_group = GroupAction(
        actions=[
            PushRosNamespace(ns),
            uins3_driver,
            depthai_driver,
        ]
    )

    return LaunchDescription([device1_group, rslidar_driver])
