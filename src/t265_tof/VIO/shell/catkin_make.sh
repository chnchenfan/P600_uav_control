# !/bin/zsh

cd ~/UAV_project 
catkin_make -DCATKIN_WHITELIST_PACKAGES="px4_realsense_bridge"
catkin_make -DCATKIN_WHITELIST_PACKAGES="realsense2_camera"