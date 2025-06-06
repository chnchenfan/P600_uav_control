# !/bin/zsh

cd ~/UAV_project #在工作区文件夹路径
catkin_make -DCATKIN_WHITELIST_PACKAGES="uav"
catkin_make -DCATKIN_WHITELIST_PACKAGES="vrpn_client_ros"