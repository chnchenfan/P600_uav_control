#include "uav/Commander.h"

Commander::Commander(ros::NodeHandle& nh,std::string model_name)
{
    int flag;
    nh.getParam("/group_flag", flag);
    if(flag==0){
        position_target_pub = nh.advertise<geometry_msgs::PoseStamped>("/wjl/set_pose/position", 10);
        yaw_target_pub = nh.advertise<std_msgs::Float64>("/wjl/set_pose/orientation", 10);//发送偏航角度
        custom_activity_pub = nh.advertise<std_msgs::String>("/wjl/set_activity/type", 10);
    }else if(flag==1){
        position_target_pub = nh.advertise<geometry_msgs::PoseStamped>("/"+model_name+"/wjl/set_pose/position", 10);
        yaw_target_pub = nh.advertise<std_msgs::Float64>("/"+model_name+"/wjl/set_pose/orientation", 10);//发送偏航角度
        custom_activity_pub = nh.advertise<std_msgs::String>("/"+model_name+"/wjl/set_activity/type", 10);
    }else{
        ROS_ERROR("参数加载失败，请检查重新启动！！！");
        exit(0);
    }
}

Commander::~Commander()
{
}

void Commander::move(double x, double y, double z)
{
    set_pose(x, y, z);
    position_target_pub.publish(pose);
}

void Commander::turn(double yaw_deg)
{
    yaw.data = yaw_deg;
    yaw_target_pub.publish(yaw);
}
void Commander::offboard()
{ 
    activity.data = "OFFBOARD";
    custom_activity_pub.publish(activity);
    // ROS_INFO("UAV pubish offboard");
}

void Commander::arm()
{
    activity.data = "ARM";
    custom_activity_pub.publish(activity);
    // ROS_INFO("UAV publish armed");
}

void Commander::disarm()
{
    activity.data = "DISARM";
    custom_activity_pub.publish(activity);
}
/*悬停*/
void Commander::hover()
{
    activity.data = "HOVER";
    custom_activity_pub.publish(activity);
}

void Commander::land()
{
    activity.data = "LAND";
    custom_activity_pub.publish(activity);
}

void Commander::takeoff()
{
    activity.data = "TAKEOFF";
    custom_activity_pub.publish(activity);  
}


void Commander::set_pose(double x, double y, double z)
{
    // FLU是基于当前的飞行位置作为为初始位置
    pose.header.stamp = ros::Time::now();
    pose.header.frame_id = "map";//这里是世界定位，也就是飞机的起飞位置
    pose.pose.position.x = x;
    pose.pose.position.y = y;
    pose.pose.position.z = z;
}







