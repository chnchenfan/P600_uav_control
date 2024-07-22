# UAV_project
这是无人机代码控制框架，包含tof和t265，以及控制无人机的代码
## 说明
- 我将必要的yaml文件放在UAV_ws/uav/config/uav_info.yaml中
	- 包含是否加入组名，tof的端口号和波特率等

## 实物遥控器说明
- SWA：7通——按下SWA，然后将自稳模式切定高模式，就执行offboard模式
- SWB：6通——无
- SWC：5通——设置飞行状态，自稳模式(飞行模式1)，定高模式(飞行模式4,Altitute)，定点模式(飞行模式6，position)
- SWD：8通——设置紧急锁桨断电