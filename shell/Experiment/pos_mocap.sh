#!/bin/bash
# 这是动捕定位的启动脚本
sudo chmod 777 /dev/ttyACM0
source /opt/ros/noetic/setup.bash
source ~/.bashrc
workplace_environment_var=~/UAV_project/devel/setup.bash
gnome-terminal --window -e "bash -c 'source $workplace_environment_var;roslaunch uav uav_yaml.launch estimator_flag:=w3; exec bash'" \
--tab -e "bash -c 'sleep 3;source $workplace_environment_var;roslaunch uav real_px4_startup.launch ns_flag:=g0; exec bash'" \
--tab -e "bash -c 'cd ~/UAV_project/src/uav/config;rosbag record -a -O uav_info; exec bash'" \
--tab -e "bash -c 'sleep 4; rostopic echo /mavros/local_position/pose; exec bash'" \
# --tab -e "bash -c 'sleep 4; rostopic echo /uav0/mavros/local_position/pose; exec bash'" \

#注意real_px4_startup.launch中的名字和波特率是否正确

## 组名说明，有组名默认/uav0
# 需要和config中的group_flag标志对应
# group_flag:0  启动  real_px4_startup.launch ns_flag:=g0  无组名
# group_flag:1  启动  real_px4_startup.launch ns_flag:=g1  有组名

## 定位说明
# 这里定位默认使用动捕定位