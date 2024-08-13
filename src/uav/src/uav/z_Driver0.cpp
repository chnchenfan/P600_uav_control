#include "uav/Driver.h"
void Judge_param_load(ros::NodeHandle nh);//通过这个函数判断yaml是否加载到参数服务器中

int main(int argc,char** argv)
{
    setlocale(LC_ALL, "");
    ros::init(argc, argv, "Driver0");
    ros::NodeHandle nh;
    Judge_param_load(nh);
    std::string model_name;
    nh.getParam("/model_name0", model_name); 
    Driver driver(nh,model_name);
    Printf_green("Driver0节点启动!!!");
    driver.start();
    return 0;
}

void Judge_param_load(ros::NodeHandle nh){
    while (!nh.hasParam("load_param_flag"))
  	{
    	ROS_INFO("等待参数加载...");
    	ros::Duration(1.0).sleep();
  	}
    ROS_INFO("yaml文件参数加载成功");
}