import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument, GroupAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node, PushRosNamespace

def generate_launch_description():
    # Paths
    uins3_launch_dir = os.path.join(get_package_share_directory('uins3_driver'), 'launch')
    rslidar_launch_dir = os.path.join(get_package_share_directory('rslidar_sdk'), 'launch')
    depthai_launch_dir = os.path.join(get_package_share_directory('depthai_ros_driver'), 'launch')
    fast_livo_launch_dir = os.path.join(get_package_share_directory('fast_livo'), 'launch')
    
    # Namespace for Device 1
    ns = 'dev1'

    # 1. uINS-3 Driver
    uins3_driver = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(uins3_launch_dir, 'uins3.launch.py'))
    )

    # 2. RoboSense Driver
    rslidar_config = os.path.join(get_package_share_directory('rslidar_sdk'), 'config', 'config.yaml')
    rslidar_driver = Node(
        package='rslidar_sdk',
        executable='rslidar_sdk_node',
        name='rslidar_sdk_node',
        output='screen',
        parameters=[{'config_path': rslidar_config}]
    )

    # 3. OAK-D Pro Driver
    depthai_config = os.path.join(get_package_share_directory('scanar_gui'), 'config', 'oak_calibration.yaml')
    depthai_driver = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(depthai_launch_dir, 'camera.launch.py')),
        launch_arguments={
            'name': 'oak', 
            'parent_frame': ns + '/base_link',
            'params_file': depthai_config
        }.items()
    )

    # 4. FAST-LIVO2 SLAM
    fast_livo_config = os.path.join(get_package_share_directory('fast_livo'), 'config', 'scanar_v2_rs.yaml')
    fast_livo_slam = Node(
        package="fast_livo",
        executable="fastlivo_mapping",
        name="laserMapping",
        parameters=[
            fast_livo_config,
            {"use_sim_time": False},
        ],
        output="screen",
        # Remap topics to match namespaced sensor outputs
        remappings=[
            ('/rslidar_points', ns + '/rslidar_points'),
            ('/imu/data', ns + '/imu/data'),
            ('/oak/rgb/image_raw', ns + '/oak/rgb/image_raw'),
        ]
    )

    # Wrap everything in a namespace group
    device1_group = GroupAction(
        actions=[
            PushRosNamespace(ns),
            uins3_driver,
            rslidar_driver,
            depthai_driver,
            fast_livo_slam,
        ]
    )

    return LaunchDescription([
        device1_group
    ])
