# uav

## English

`uav` is the main flight-control package in `UAV_project`. It provides the UAV-side execution chain, state handling, guideflight control, dynamic manual command publishing, and localization-related launch entrypoints.

### Main executables

- `Driver0`
- `desired_uav_fly`
- `uav_guidefly_control`
- `dynamic_guidefly`
- `pos_estimator0`
- `gazebo_pose0`

### Main launch files

- `launch/desired_uav_fly.launch`
- `launch/dynamic_guidefly.launch`
- `launch/uav_drvier_guidefly.launch`
- `launch/uav_yaml.launch`

### Main interfaces

Consumes:

- `/wjl/guidefly/pose_d`

Provides or relies on:

- `/mavros/local_position/pose`
- `/mavros/state`
- `/mavros/vision_pose/pose`

Custom message and service:

- `msg/xyz_yaw_d.msg`
- `srv/desired_start.srv`

### Role in the full system

This package is the UAV execution layer. It receives task-level pose references from `p600_arm_control` and turns them into actual flight actions through the PX4/MAVROS chain.

### Where it is used

- Shared UAV execution path for experiments 1 to 4
- Parameter loading and estimator selection through `uav_yaml.launch`
- Manual dynamic reference publication through `dynamic_guidefly`

## 中文

`uav` 是 `UAV_project` 中的核心飞行控制包，负责无人机侧执行链、状态管理、指点飞行控制、动态手工期望发布以及定位相关 launch 入口。

### 主要可执行节点

- `Driver0`
- `desired_uav_fly`
- `uav_guidefly_control`
- `dynamic_guidefly`
- `pos_estimator0`
- `gazebo_pose0`

### 主要 launch 文件

- `launch/desired_uav_fly.launch`
- `launch/dynamic_guidefly.launch`
- `launch/uav_drvier_guidefly.launch`
- `launch/uav_yaml.launch`

### 主要接口

订阅:

- `/wjl/guidefly/pose_d`

提供或依赖:

- `/mavros/local_position/pose`
- `/mavros/state`
- `/mavros/vision_pose/pose`

自定义消息与服务:

- `msg/xyz_yaw_d.msg`
- `srv/desired_start.srv`

### 在整套系统中的角色

这个包是无人机执行层，负责接收 `p600_arm_control` 发布的任务级位姿期望，并通过 PX4/MAVROS 链路将其转成实际飞行动作。

### 使用场景

- 作为实验 1 到实验 4 的统一无人机执行链
- 通过 `uav_yaml.launch` 完成参数加载和定位源选择
- 通过 `dynamic_guidefly` 做手工动态期望发布
