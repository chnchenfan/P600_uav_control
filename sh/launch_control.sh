#!/bin/bash
#启动mavros,px4,tof和T265
#这是在修改了固定串口号-tof
sudo chmod 777 /dev/tof
sudo chmod 777 /dev/ttyACM0
#说明,注意修改路径
source /opt/ros/noetic/setup.bash
source /home/ywb/.bashrc
uav_workplace_environment_var=/home/wjl/UAV_project/UAV_ws/devel/setup.bash
gnome-terminal --window -e "bash -c 'roscore; exec bash'" \
--tab -e "bash -c 'sleep 1;source $uav_workplace_environment_var;roslaunch uav uav_yaml.launch; exec bash'" \
--tab -e 'bash -c "sleep 2; roslaunch mavros px4.launch; exec bash"' \
--tab -e 'bash -c "sleep 2; source /home/wjl/UAV_project/T265_tof_ws/devel/setup.bash && roslaunch px4_realsense_bridge bridge.launch; exec bash"' \
--tab -e 'bash -c "sleep 4; rostopic echo /mavros/local_position/pose; exec bash"' \
# gnome-terminal --window -e 'bash -c "sleep 6; rostopic echo iris_0/mavros/local_position/pose; exec bash"' \  #有组名