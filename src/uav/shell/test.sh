# !/bin/bash
workplace_environment_var=~/UAV_project/devel/setup.bash
gnome-terminal --window -e "bash -c 'sleep 1;source $workplace_environment_var && roslaunch uav uav_yaml.launch; exec bash'" \
--tab -e "bash -c 'sleep 3;source $workplace_environment_var;rosrun uav test1; exec bash'" \
