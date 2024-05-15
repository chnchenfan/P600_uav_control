# !/bin/zsh
#说明
#这个脚本文件是在功能包的launch文件夹中
#适用结构：工作区/src/功能包/launch/*.sh
cd ../../..   #工作区路径
workplace_environment_var=$(pwd)/devel/setup.zsh #注意看后缀是zsh还是bash

gnome-terminal --window -e "zsh -c 'sleep 1;roslaunch px4 uav_v2.launch; exec zsh'" \
gnome-terminal --window -e "zsh -c 'sleep 9;source $workplace_environment_var;rosrun uav test_uav0; exec zsh'" \
gnome-terminal --window -e "zsh -c 'sleep 9;source $workplace_environment_var;rosrun uav Driver_0; exec zsh'" \




