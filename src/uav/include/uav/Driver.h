
// ros
#include <iostream>
#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <geometry_msgs/PoseStamped.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Eigen>
#include <Eigen/StdVector>
#include <tf/tf.h>

#include <math.h>
// mavros
#include <mavros_msgs/PositionTarget.h>
#include <mavros_msgs/State.h>//状态
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/CommandBool.h>
#include "other/Print_.h"
#define pi 3.14159
#define TAKEOFF_HEIGHT 0.5
#define CAMERA_INT_HEIGHT 0.4

using namespace std;
//主要是
class Driver
{
  
public:
  Driver(ros::NodeHandle& nh,std::string model_name);
  ~Driver();
  //模式设置
  void land();
  void offboard();
  void arm();
  void disarm();
  void hover();
  void takeoff();
  void set_target_pose(double x, double y, double z, double yaw);
  void start();
  bool arm_offb_detection();//offboard模式和起飞检测
  
private:
  
  /*订阅器*/
  ros::Subscriber local_pose_sub;//当前位置订阅
  ros::Subscriber mavros_sub;//状态订阅
  ros::Subscriber set_target_position_sub;//设置目标位置订阅
  ros::Subscriber set_target_yaw_sub;//偏航角订阅
  ros::Subscriber custom_activity_sub;//当前模式名订阅

  
  /*发布器*/
  ros::Publisher local_target_pub;//目标位置信息发布
  
  /*客户端*/
  ros::ServiceClient arming_client;
  ros::ServiceClient set_mode_client;
  
  /*变量*/
  mavros_msgs::State mavros_state;//当前状态
  mavros_msgs::SetMode set_mode;//模式设置
  mavros_msgs::CommandBool arm_cmd;
  mavros_msgs::PositionTarget cur_target_pose;//作为发布当前的目标位置
  //当前位置信息
  double cur_position_x;
  double cur_position_y;
  double cur_position_z;
  double yaw_rad ;
  double cur_heading_rad;//当前偏航角
  bool landing_requested{false};
  bool local_pose_received{false};
    //回调函数
  void local_pose_callback(const geometry_msgs::PoseStampedConstPtr& msg);
  void mavros_state_callback(const mavros_msgs::StateConstPtr& msg);
  void set_target_position_callback(const geometry_msgs::PoseStampedConstPtr& msg);
  void set_target_yaw_callback(const std_msgs::Float64ConstPtr& msg);
  void custom_activity_callback(const std_msgs::StringConstPtr& msg);
};
