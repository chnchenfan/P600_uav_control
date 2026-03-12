# vrpn_client_ros

## English

`vrpn_client_ros` is the mocap access and bridge package used in this project. It runs the VRPN client, publishes tracker poses into ROS, and bridges the main UAV mocap pose into MAVROS vision input.

### Main executables

- `vrpn_tracker_node`
- `vrpn_client_node`
- `vrpn2mavros`

### Main launch files

- `launch/sample.launch`
- `launch/vrpn_wjl.launch`

### Main interfaces

Publishes:

- `/vrpn_client_node/Tracker0/pose`
- `/vrpn_client_node/<tracker_name>/pose` for additional tracked rigid bodies exposed by the VRPN server

Bridges:

- mocap pose to `/mavros/vision_pose/pose` through `vrpn2mavros`

### Role in the full system

This package is the mocap integration layer. It provides the UAV rigid-body pose for mocap-based flight and also exposes additional rigid bodies, such as ring targets used by experiment 4.

### Where it is used

- Included by `uav/launch/uav_yaml.launch` when mocap mode is selected
- Used by experiment 3 for base mocap truth
- Used by experiment 4 for both base mocap truth and ring position input

## 中文

`vrpn_client_ros` 是本项目中的动捕接入与桥接包，负责运行 VRPN 客户端、将 tracker 位姿发布到 ROS，并把主无人机刚体的动捕位姿桥接到 MAVROS 的视觉定位输入。

### 主要可执行节点

- `vrpn_tracker_node`
- `vrpn_client_node`
- `vrpn2mavros`

### 主要 launch 文件

- `launch/sample.launch`
- `launch/vrpn_wjl.launch`

### 主要接口

发布:

- `/vrpn_client_node/Tracker0/pose`
- 如果 VRPN 服务器暴露了额外刚体，还会发布 `/vrpn_client_node/<tracker_name>/pose`

桥接:

- 通过 `vrpn2mavros` 将动捕位姿送到 `/mavros/vision_pose/pose`

### 在整套系统中的角色

这个包是动捕接入层，负责给基于动捕的飞行提供无人机刚体位姿，也负责向系统暴露额外的刚体目标，例如实验四中的挂环。

### 使用场景

- 在 `uav/launch/uav_yaml.launch` 选择动捕模式时被包含
- 实验 3 中用于基座动捕真值
- 实验 4 中同时用于基座动捕真值和挂环位置输入
