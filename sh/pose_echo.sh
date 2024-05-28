# !/bin/bash
# 打印位置信息
gnome-terminal --window -e 'bash -c "sleep 1; rostopic echo /mavros/local_position/pose; exec bash"' \