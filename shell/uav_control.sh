# !/bin/bash
#说明,注意修改路径
workplace_environment_var=~/UAV_project/devel/setup.bash
gnome-terminal --window -e "bash -c 'source $workplace_environment_var;roslaunch uav real_uav_drvier.launch; exec bash'" \