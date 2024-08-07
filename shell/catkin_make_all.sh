#!/bin/bash
gnome-terminal --window -e "bash -c 'cd ~/UAV_project/UAV_ws;catkin_make; exec bash'" \
--tab -e "bash -c 'cd ~/UAV_project/T265_tof_ws;catkin_make; exec bash'" \
