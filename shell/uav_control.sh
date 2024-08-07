# !/bin/bash
#说明,注意修改路径
workplace_environment_var=~/UAV_project/UAV_ws/devel/setup.bash
gnome-terminal --window -e "bash -c 'sleep 5;source $workplace_environment_var;rosrun uav uav; exec bash'" \
--tab -e "bash -c 'sleep 5;source $workplace_environment_var;roslaunch uav uav_Drive.launch; exec bash'" \
