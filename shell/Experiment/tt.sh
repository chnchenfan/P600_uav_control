#!/bin/bash
# 动捕测试启动脚本，支持显式指定刚体名和 VRPN server IP。
# 用法：
#   bash tt.sh Tracker0
#   bash tt.sh arm_base 10.1.1.198
source ~/.bashrc
workplace_environment_var=~/UAV_project/devel/setup.bash
mocap_object="${1:-Tracker0}"
server_name="${2:-${VRPN_SERVER:-192.168.18.12}}"
echo "Using mocap_object=$mocap_object"
echo "Using server_name=$server_name"
gnome-terminal --window -e "bash -c 'source $workplace_environment_var;roslaunch vrpn_client_ros vrpn_wjl.launch mocap_object:=$mocap_object server_name:=$server_name; exec bash'"
