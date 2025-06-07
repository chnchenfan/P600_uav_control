# 这个脚本是为了保证px4的版本是否和当前系统匹配,正对仿真
cd ~/PX4-Autopilot
current_version=$(git rev-parse HEAD)
match_version=07a993234cc159f49b3bc3b34b1483480c7421c4
if [ "$current_version" != "$match_version" ]; then
    echo "px4版本不匹配,正在切换到匹配版本"
    git checkout $match_version
    git submodule update --init --recursive
    echo "切换完成编译中"
    make px4_sitl gazebo
else
    echo "px4版本匹配"
    # 检查build文件夹是否存在
    if [ -d "build" ]; then
        echo "build文件夹存在，已编译过，可直接运行"
    else
        echo "build文件夹不存在，需要重新编译"
        make px4_sitl gazebo
    fi
fi
