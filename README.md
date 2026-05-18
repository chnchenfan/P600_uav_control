# UAV_project

P600 无人机飞行控制侧 catkin 工作空间。该仓库负责 PX4/MAVROS 执行链、无人机状态管理、定位接入和动捕到 MAVROS 的桥接；四个实验的任务逻辑与机械臂控制位于配套仓库 ；一般由p600_arm_control启动
[p600_arm_control](https://github.com/chnchenfan/p600_arm_control)。

- GitHub: <https://github.com/chnchenfan/P600_uav_control>
- 配套机械臂仓库: <https://github.com/chnchenfan/p600_arm_control>
- 推荐环境: Ubuntu 18.04 + ROS Melodic

## 目录结构

- `src/uav`: 无人机消息/服务、PX4/MAVROS 执行、定位与飞行控制节点
- `src/vrpn_client_ros`: VRPN 动捕客户端和 `vrpn2mavros` 桥接节点
- `src/t265_tof`: T265/ToF 相关定位组件
- `shell/Compile`: 工作空间编译脚本
- `shell/Experiment`: 真机定位、动捕和飞控启动脚本
- `shell/Simulation`: 仿真启动脚本
- `shell/Plot`: 绘图辅助脚本

## 系统角色

本仓库是“飞控执行层”。它不生成实验轨迹，而是消费 `p600_arm_control` 发布的无人机期望：

- `/wjl/guidefly/pose_d`

该消息由 `uav/msg/xyz_yaw_d.msg` 定义，主要包含：

- `x_d`
- `y_d`
- `z_d`
- `yaw_d`
- `land_flag`

飞控侧执行链会向系统提供反馈：

- `/mavros/local_position/pose`
- `/mavros/local_position/velocity_local`
- `/mavros/vision_pose/pose`
- `/mavros/state`
- `/mavros/imu/data`

主链路：

```text
p600_arm_control 实验节点
  -> /wjl/guidefly/pose_d
  -> UAV_project/desired_uav_fly
  -> PX4/MAVROS
  -> /mavros/* 反馈
  -> p600_arm_control 实验节点
```

## 编译

脚本内默认使用 `~/UAV_project`。如果仓库不在 home 目录下，建议先建立软链接或按实际路径手动进入工作空间编译。

```bash
cd ~/UAV_project
bash shell/Compile/catkin_make_all.sh
source devel/setup.bash
```

编译脚本顺序为：

1. `realsense2_camera`
2. `px4_realsense_bridge`
3. `uav`
4. `vrpn_client_ros`

只编译无人机主包时可运行：

```bash
bash shell/Compile/catkin_make_uav.sh
```

`p600_arm_control` 的 `arm_control` 包会引用本工作空间生成的消息/服务头文件，因此通常先编译本仓库，再编译机械臂侧仓库。

## 飞控执行链

主要 launch：

```bash
roslaunch uav desired_uav_fly.launch
```

该 launch 启动：

- `Driver0`: PX4/MAVROS 驱动与底层命令通道
- `desired_uav_fly`: 接收 `/wjl/guidefly/pose_d` 并执行任务期望

关键源码：

- `src/uav/src/desired_uav_fly.cpp`: 任务期望执行入口，调用 `/wjl/start/uav_desired` 启动实验
- `src/uav/src/uav/Uav_info.cpp`: 无人机状态、setpoint、降落标志和期望订阅逻辑
- `src/uav/src/uav/Driver.cpp`: MAVROS 命令封装
- `src/uav/src/uav/z_Driver0.cpp`: `Driver0` 节点入口
- `src/uav/src/uav_guidefly_control.cpp`: 手动/指点飞行控制入口
- `src/uav/src/dynamic_guidefly.cpp`: dynamic reconfigure 指点入口

常用 launch：

- `src/uav/launch/uav_yaml.launch`: 加载无人机参数、定位方式和动捕配置
- `src/uav/launch/desired_uav_fly.launch`: 期望飞行执行链
- `src/uav/launch/dynamic_guidefly.launch`: 动态指点/调参
- `src/uav/launch/uav_drvier_guidefly.launch`: 驱动与指点控制组合入口
- `src/uav/launch/physical/real_px4_startup.launch`: 真机 PX4/MAVROS 启动
- `src/uav/launch/simulation/gazebo_px4_startup.launch`: Gazebo/PX4 仿真启动
