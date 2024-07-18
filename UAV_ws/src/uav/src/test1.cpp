#include <iostream>
#include <ros/ros.h>
//通过这个函数判断yaml是否加载到参数服务器中
void Judge_param_load(ros::NodeHandle nh){
    while (!nh.hasParam("load_param_flag"))
  	{
    	ROS_INFO("等待参数加载...");
    	ros::Duration(1.0).sleep();
  	}
}

int main(int argc, char *argv[])
{   
    setlocale(LC_ALL,"");
    ros::init(argc, argv, "ttt");
    ros::NodeHandle nh;
    Judge_param_load(nh);
    int p;
    nh.param("/Pos_PID/arm_joint1/P44", p,434);//具体的哪一个串口
    std::cout<<"  p："<<p<<std::endl;

}