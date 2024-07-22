#pragma once
#include <math.h>
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
#include <unistd.h>
#include <ros/time.h>
#include <ros/duration.h>
#include <string>
#include "Uav_info.h"
#include "Commander.h"
#include "Driver.h"
#include "common.h"


class Uav_info
{
public:
    std::string model_name;
    Commander cmd;
    mavros_msgs::State current_mavros_state;
    geometry_msgs::Point setpoint_pos;//记录当前的期望值
    Uav_info(ros::NodeHandle& nh,std::string model_name);
    ~Uav_info();
    bool isArrived();//判断当前位置和目标位置是否小于某一阈值
    void IsArrived(double x,double y,double z);//（基于初始位置）判断无人机是否到达目标点，没有则发送期望信息
    void Set_arm_offboard();//设置起飞进入offboard
    void Set_pose(double x,double y,double z);//发送世界坐标系的期望位置
    bool RC_control;//遥控器连接状态
private:
    ros::NodeHandle nh;
    ros::Subscriber activity_type;//模式状态订阅
    ros::Subscriber local_pose_sub;//当前位置订阅
    ros::Subscriber mavros_state_sub;//状态订阅
    ros::Subscriber rc_sub;//遥控器信息订阅


    Eigen::Vector3d currentPos;
    Eigen::Vector3d targetPos;
    Eigen::Vector3d biasPos;//当前点和目标点之间的向量


    double cur_position_x;
    double cur_position_y;
    double cur_position_z;

    
    void mr_localpose_callback(const geometry_msgs::PoseStampedConstPtr& localposemsg);
    void mr_state_callback(const mavros_msgs::State::ConstPtr& mavrosstatemsg);
    void key_control_cb(const boost::shared_ptr<const geometry_msgs::Point>& msg);
    void RC_callback(const mavros_msgs::RCIn::ConstPtr& RCmsg);
};

