//头文件
#include <ros/ros.h>

#include <iostream>
#include <Eigen/Eigen>
#include "state_from_mavros.h"
#include "math_utils.h"
#include "prometheus_control_utils.h"
#include "message_utils.h"
class Pos_estimator{
public:
    Pos_estimator(ros::NodeHandle& nh,std::string model_name);
    void run();
private:
    int estimator_num;
    geometry_msgs::PoseStamped vision_pose;
    ros::Publisher vision_pose_pub;//发布
    ros::Subscriber gazebo_pose_sub;
    void gazebo_cb(const geometry_msgs::PoseStampedConstPtr& msg);
};