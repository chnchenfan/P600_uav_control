#include "realsense/px4_realsense_bridge.h"
using namespace bridge;
//通过这个函数判断yaml是否加载到参数服务器中
void Judge_param_load(ros::NodeHandle nh){
    while (!nh.hasParam("/load_param_flag"))
  	{
      std::cout<<!nh.hasParam("/load_param_flag")<<std::endl;
    	ROS_INFO("t265_tof等待参数加载...");
    	ros::Duration(1.0).sleep();
  	}
    ROS_INFO("t265_tof参数加载成功");
}

int main(int argc, char** argv) {
    setlocale(LC_ALL,"");
    ros::init(argc, argv, "PX4_realsense_bridge_node");
    ros::NodeHandle nh("~");
    Judge_param_load(nh);
    std::string model_name;
    nh.getParam("/model_name0", model_name);
    ROS_INFO("T265+tof节点启动");  
    PX4_Realsense_Bridge Bridge(nh,model_name);
    ros::spin();
    Bridge.worker_.join();
    return 0;
}
