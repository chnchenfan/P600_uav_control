# !/bin/bash
workplace_environment_var=~/UAV_project/devel/setup.bash
gnome-terminal --window -e "bash -c 'roscore; exec bash'" \
--tab -e "bash -c 'sleep 2;source $workplace_environment_var;roslaunch uav uav_yaml.launch; exec bash'" \
--tab -e "bash -c 'sleep 2.1;roslaunch px4 uav_v2.launch; exec bash'" \
--tab -e "bash -c 'sleep 6;source $workplace_environment_var;rosrun uav uav_gazebo; exec bash'" \
--tab -e "bash -c 'sleep 3;source $workplace_environment_var;roslaunch uav uav_Drive.launch; exec bash'" \
--tab -e "bash -c 'cd ~/UAV_project/src/uav/config;rosbag record -a -O uav_info; exec bash'" \
# --tab -e "bash -c 'sleep 2;roslaunch px4 uav_v1.launch; exec bash'" #有组名
#使用前及的看是否使用了组名