#include "Driver.h"
//通过这个函数判断yaml是否加载到参数服务器中
void Judge_param_load(ros::NodeHandle nh){
    while (!nh.hasParam("load_param_flag"))
  	{
    	ROS_INFO("等待参数加载...");
    	ros::Duration(1.0).sleep();
  	}
    ROS_INFO("加载成功");
}

int main(int argc,char** argv)
{
    setlocale(LC_ALL, "");
    ros::init(argc, argv, "driver_0");
    ros::NodeHandle nh;
    Judge_param_load(nh);
    std::string modle_name;
    nh.getParam("modle_name", modle_name);;  
    Driver driver(nh,modle_name);
    driver.start();
    return 0;
}