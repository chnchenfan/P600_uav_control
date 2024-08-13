#!/bin/bash
# 这是t265和tof定位的启动
#启动mavros,px4,tof和T265
#这是在修改了固定串口号-tof和arm_usb
echo aaa | sudo -S sudo chmod 777 /dev/tof
sudo chmod 777 /dev/ttyACM0
source /opt/ros/noetic/setup.bash
source ~/.bashrc
workplace_environment_var=~/UAV_project/devel/setup.bash
gnome-terminal --window -e "bash -c 'roscore; exec bash'" \
--tab -e "bash -c 'sleep 1;source $workplace_environment_var;roslaunch uav uav_yaml.launch estimator_flag:=w2; exec bash'" \
--tab -e "bash -c 'sleep 2;source $workplace_environment_var;roslaunch uav real_px4_startup.launch ns_flag:=g0; exec bash'" \
--tab -e "bash -c 'cd ~/UAV_project/src/uav/config;rosbag record -a -O uav_info; exec bash'" \
--tab -e "bash -c 'sleep 4; rostopic echo /mavros/local_position/pose; exec bash'" \
# --tab -e "bash -c 'sleep 4; rostopic echo /uav0/mavros/local_position/pose; exec bash'" \


## 组名说明，有组名默认/uav0
# 需要和config中的group_flag标志对应
# group_flag:0  启动  real_px4_startup.launch ns_flag:=g0  无组名
# group_flag:1  启动  real_px4_startup.launch ns_flag:=g1  有组名

## 定位说明
# 这里定位默认使用t265+tof定位