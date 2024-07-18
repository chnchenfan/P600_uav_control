# !/bin/bash
#说明
#这个脚本文件是在功能包的launch文件夹中
#适用结构：工作区/src/功能包/launch/*.sh
cd ../../..   #工作区路径
workplace_environment_var=$(pwd)/devel/setup.bash 
gnome-terminal --window -e "bash -c 'sleep 1;source $workplace_environment_var;roslaunch uav uav_yaml.launch; exec bash'" \
--tab -e "bash -c 'sleep 3;source $workplace_environment_var;rosrun uav test1; exec bash'" \
