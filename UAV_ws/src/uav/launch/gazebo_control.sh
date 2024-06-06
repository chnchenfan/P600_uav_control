# !/bin/bash
#说明
#这个脚本文件是在功能包的launch文件夹中
#适用结构：工作区/src/功能包/launch/*.sh
cd ../../..   #工作区路径
workplace_environment_var=$(pwd)/devel/setup.bash #注意看后缀是zsh还是bash
gnome-terminal --window -e "bash -c 'sleep 1;source $workplace_environment_var;roslaunch uav uav_yaml.launch; exec bash'" \
gnome-terminal --window -e "bash -c 'sleep 2;roslaunch px4 uav_v2.launch; exec bash'" \   #无组名
# gnome-terminal --window -e "bash -c 'sleep 2;roslaunch px4 uav_v1.launch; exec bash'" \   #有组名
gnome-terminal --window -e "bash -c 'sleep 3;source $workplace_environment_var;rosrun uav uav; exec bash'" \
gnome-terminal --window -e "bash -c 'sleep 3;source $workplace_environment_var;roslaunch uav uav_Drive.launch; exec bash'" \
