#include "uav/Uav_info.h"
//通过这个函数判断yaml是否加载到参数服务器中
void Judge_param_load(ros::NodeHandle nh){
    while (!nh.hasParam("/load_param_flag"))
  	{
    	ROS_INFO("等待参数加载...");
    	ros::Duration(1.0).sleep();
  	}
    ROS_INFO("加载成功");
}

//主要是解锁并进入offboard模式，接着就是IsArrived判断是否到达指定位置，iris.Set_pose(x,y,z);设置期望位置
//判断是否到达指定位置

//仿真测试
void Gazebo_test(Uav_info &iris){
    ros::Rate r(30);
    while(ros::ok()){ 
        //解锁并进入offboard模式并飞到起点上方
        ROS_INFO("即将解锁");
        iris.Set_arm_offboard();
        ROS_INFO("offboard 切换成功！！！！");
        ROS_INFO("炸鸡正在去起点上方");
        iris.IsArrived(iris.setpoint_pos.x,iris.setpoint_pos.y,iris.setpoint_pos.z);
        //正方形飞行
        iris.IsArrived(0,0,1);
        iris.IsArrived(1,0,1);
        iris.IsArrived(1,1,1);
        iris.IsArrived(0,1,1);
        iris.IsArrived(0,0,1);
        iris.IsArrived(0,0,0.5);        
        break;
        r.sleep();
        ros::spinOnce();  
    } 
}

int main(int argc, char *argv[])
{   
    setlocale(LC_ALL,"");
    ros::init(argc, argv, "uav0");
    ros::NodeHandle nh;

    Judge_param_load(nh);//判断是否加载yaml参数
    std::string modle_name;
    nh.getParam("modle_name", modle_name);;    
    Uav_info iris0(nh,modle_name);
    iris0.setpoint_pos.z=0.5;
    Gazebo_test(iris0);//仿真测试
    // path_planning(iris0);//实物测试
    ROS_INFO("即将降落");
    iris0.cmd.land();
    ROS_INFO("降落成功！！！！");
    //让Driver节点一直运行
    ros::spin();
}