# !/bin/bash
# 说明：实验模式下默认只启动飞行执行链路，避免 dynamic_guidefly 覆盖 /wjl/guidefly/pose_d。
# 如需手工调试 dynamic_reconfigure，请显式设置 ENABLE_DYNAMIC_GUIDEFLY=1。
workplace_environment_var=~/UAV_project/devel/setup.bash
enable_dynamic_guidefly=${ENABLE_DYNAMIC_GUIDEFLY:-0}

if [ "$enable_dynamic_guidefly" = "1" ]; then
  gnome-terminal --window -e "bash -c 'source $workplace_environment_var;roslaunch uav uav_drvier_guidefly.launch; exec bash'"   --tab -e "bash -c 'source $workplace_environment_var;sleep 2;roslaunch uav dynamic_guidefly.launch; exec bash'"
else
  gnome-terminal --window -e "bash -c 'source $workplace_environment_var;roslaunch uav uav_drvier_guidefly.launch; exec bash'"
fi
# 定位说明
# w0:gps定位,w1:gazebo真值定位,w2:tof+T265定位,w3:动捕定位
