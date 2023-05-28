import os

from ament_index_python.packages import get_package_share_directory

import launch
import launch_ros.actions

def generate_launch_description():
    return launch.LaunchDescription([
        launch_ros.actions.Node(
            package='odrive_simple', 
			executable='odrive_s',
            name='odrive_s_node', 
			parameters=[{'my_debug':'True'}],
			output='screen',
            #remappings=[('/cmd_vel', '/demo/cmd_vel'),]            
            ),
        launch_ros.actions.Node(
            package='bigbottools', executable='dosquare',
            name='dosquare_node', parameters=[config_filepath],
			output='screen',
            #remappings=[('/cmd_vel', '/demo/cmd_vel'),]            
            ),
		launch.actions.ExecuteProcess(
					cmd=['ros2', 'bag', 'record', '/odom','/cmd_vel','--lz4' ],
					output='screen'
				)
	])
