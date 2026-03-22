#!/bin/bash
# 动捕测试启动脚本，默认 Tracker0；也支持显式指定刚体名。
source ~/.bashrc
workplace_environment_var=~/UAV_project/devel/setup.bash
mocap_object="${1:-Tracker0}"
gnome-terminal --window -e "bash -c 'source $workplace_environment_var;roslaunch vrpn_client_ros vrpn_wjl.launch mocap_object:=$mocap_object; exec bash'"
