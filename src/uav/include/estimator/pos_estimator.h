//头文件
#include <ros/ros.h>

#include <iostream>
#include <geometry_msgs/PoseStamped.h>
#include "other/Print_.h"
class Pos_estimator{
public:
    Pos_estimator(ros::NodeHandle& nh,std::string model_name);
    void run();
private:
    std::string estimator_flag;
    geometry_msgs::PoseStamped gazebo_pose;
    geometry_msgs::PoseStamped mocap_pose;
    geometry_msgs::PoseStamped vision_pose;
    ros::Publisher vision_pose_pub;//发布
    ros::Subscriber gazebo_pose_sub;
    ros::Subscriber mocap_pose_sub;
    void mocap_cb(const geometry_msgs::PoseStampedConstPtr& msg);
    void gazebo_cb(const geometry_msgs::PoseStampedConstPtr& msg);
};