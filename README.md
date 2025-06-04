# UAV_project
这是无人机代码控制框架，uav主要是作为控制无人机信息
## 说明
### 下载说明
- 在主目录克隆下载
```cpp
git clone http://localhost:3000/UAV_control/UAV_project.git
```
### 配置说明
- 运行前保证与PX4代码版本保持一致，运行px4_version.sh即可
- 我将必要的yaml文件放在UAV_ws/uav/config/uav_info.yaml中
	- 包含是否加入组名，tof的端口号和波特率设置等

- 在仿真使用中必须添加环境变量，实物可以不同
	- 在.bashrc中环境变量添加：source ~/UAV_project/devel/setup.bash
	- 在.zshrc中环境变量添加：source ~/UAV_project/devel/setup.zsh

## 实物遥控器说明
**遥控器1**
- 这个遥控器只有4个通道
- `SWC`：设置飞行状态
	- 自稳模式:飞行模式1
	- 定高模式:飞行模式4,Altitute
	- 定点模式:飞行模式6,position
- `SWA`:电机断电（向下）

**遥控器2**
- 这个遥控器至少有8个通道
- SWA:设置飞行状态
	- 自稳模式:飞行模式1
	- 定高模式:飞行模式4,Altitute
	- 定点模式:飞行模式6,position
- SWC:电机断电（向下）

# 代码参数说明
- `src/uav/config/uav_info.yaml`
	- 与组名相关
	- 定位信息:与tof参数以及动捕信息参数相关
- 定位选择
	- 对于定位选择,启动`uav_yaml.launch`文件时设置参数选择