#include "../../include/PX4_realsense_bridge/PX4_realsense_bridge.h"
#include <sensor_msgs/Range.h>
#include <mySerial.h>
using namespace bridge;
//通过这个函数判断yaml是否加载到参数服务器中
void Judge_param_load(ros::NodeHandle nh){
    std::cout<<nh.hasParam("/load_param_flag")<<std::endl;
    while (!nh.hasParam("/load_param_flag"))
  	{
      std::cout<<!nh.hasParam("/load_param_flag")<<std::endl;
    	ROS_INFO("t265_tof等待参数加载...");
    	ros::Duration(1.0).sleep();
  	}
    ROS_INFO("加载成功");
}

int main(int argc, char** argv) {
    setlocale(LC_ALL,"");
    ros::init(argc, argv, "PX4_realsense_bridge_node");
    ros::NodeHandle nh("~");
    Judge_param_load(nh);

    std::string modle_name;
    nh.getParam("modle_name", modle_name);;    
    PX4_Realsense_Bridge Bridge(nh,modle_name);
    ros::spin();
    Bridge.worker_.join();
    return 0;
}
