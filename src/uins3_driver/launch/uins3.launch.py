import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    config_dir = os.path.join(get_package_share_directory('uins3_driver'), 'config')
    param_config = os.path.join(config_dir, 'uins3.yaml')

    return LaunchDescription([
        Node(
            package='uins3_driver',
            executable='uins3_node',
            name='uins3_driver',
            output='screen',
            parameters=[param_config]
        )
    ])