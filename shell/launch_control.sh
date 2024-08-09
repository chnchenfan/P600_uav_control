#!/bin/bash
# 这是t265和tof定位的启动
#启动mavros,px4,tof和T265
#这是在修改了固定串口号-tof和arm_usb
echo 123 | sudo -S sudo chmod 777 /dev/tof
sudo chmod 777 /dev/ttyACM0

source /opt/ros/noetic/setup.bash
source ~/.bashrc
uav_environment=~/UAV_project/devel/setup.bash
gnome-terminal --window -e "bash -c 'roscore; exec bash'" \
--tab -e "bash -c 'sleep 1;source $uav_environment;roslaunch uav uav_yaml.launch; exec bash'" \
--tab -e "bash -c 'sleep 2; roslaunch mavros px4.launch; exec bash'" \
--tab -e "bash -c 'sleep 2; source $uav_environment && roslaunch px4_realsense_bridge bridge.launch; exec bash'" \
--tab -e "bash -c 'cd ~/UAV_project/src/uav/config;rosbag record -a -O uav_info; exec bash'" \
--tab -e "bash -c 'sleep 4; rostopic echo /mavros/local_position/pose; exec bash'" \