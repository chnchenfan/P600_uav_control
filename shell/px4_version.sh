# 这个脚本是为了保证px4的版本是否和当前系统匹配
cd ~/PX4-Autopilot
current_version=$(git rev-parse HEAD)
match_version=dd9adc894963654fc1135adcef0a15742adb6203
if [ "$current_version" != "$match_version" ]; then
    echo "px4版本不匹配,正在切换到匹配版本"
    git checkout $match_version
    git submodule update --init --recursive
    echo "切换完成编译中"
    make px4_sitl gazebo
else
    echo "px4版本匹配,可直接运行编译代码"
fi
