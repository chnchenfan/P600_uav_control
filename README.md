# UAV_project
这是无人机代码控制框架，uav主要是作为控制无人机信息
## 说明
### 下载说明
- 在主目录克隆下载
```cpp
git clone https://gitee.com/whoarewang/uav_project.git UAV_project
```
### 配置说明
- 我将必要的yaml文件放在UAV_ws/uav/config/uav_info.yaml中
	- 包含是否加入组名，tof的端口号和波特率设置等

- 在仿真使用中必须添加环境变量，实物可以不同
	- 在.bashrc中环境变量添加：source ~/UAV_project/devel/setup.bash
	- 在.zshrc中环境变量添加：source ~/UAV_project/devel/setup.zsh

## 实物遥控器说明
- SWA：7通——按下SWA，然后将自稳模式切定高模式，就执行offboard模式
- SWB：6通——无
- SWC：5通——设置飞行状态，自稳模式(飞行模式1)，定高模式(飞行模式4,Altitute)，定点模式(飞行模式6，position)
- SWD：8通——设置紧急锁桨断电
