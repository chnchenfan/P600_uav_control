#!/bin/bash
# 动捕定位启动脚本。
# 用法：
#   bash pos_mocap.sh                # 默认 Tracker0
#   bash pos_mocap.sh exp2           # 实验二 -> arm_base
#   bash pos_mocap.sh exp4           # 实验四 -> arm_target
#   bash pos_mocap.sh arm_base       # 直接指定刚体名

sudo chmod 777 /dev/ttyTHS0
source ~/.bashrc
workplace_environment_var=~/UAV_project/devel/setup.bash

selection="${1:-Tracker0}"
case "$selection" in
  1|exp1|experiment1|Tracker0|tracker0)
    mocap_object="Tracker0"
    ;;
  2|exp2|experiment2|arm_base)
    mocap_object="arm_base"
    ;;
  3|exp3|experiment3)
    mocap_object="Tracker0"
    ;;
  4|exp4|experiment4|arm_target)
    mocap_object="arm_target"
    ;;
  *)
    mocap_object="$selection"
    ;;
esac

echo "Using mocap_object=$mocap_object"

gnome-terminal --window -e "bash -c 'source $workplace_environment_var;roslaunch uav uav_yaml.launch estimator_flag:=w3 mocap_object:=$mocap_object; exec bash'" --tab -e "bash -c 'sleep 3;source $workplace_environment_var;roslaunch uav real_px4_startup.launch ns_flag:=g0; exec bash'" --tab -e "bash -c 'sleep 5;cd ~/UAV_project/src/uav/config;rosbag record -a -O uav_mocap; exec bash'" --tab -e "bash -c 'sleep 4; rostopic echo /mavros/local_position/pose; exec bash'"

# 注意 real_px4_startup.launch 中的名字和波特率是否正确
# group_flag:0  对应 ns_flag:=g0  无组名
# group_flag:1  对应 ns_flag:=g1  有组名
