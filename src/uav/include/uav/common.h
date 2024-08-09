#pragma once

// std
#include <iostream>
#include <queue>
#include <vector>

// ros
#include <ros/ros.h>
#include <std_msgs/Header.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>

#include <geometry_msgs/Point.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/PoseStamped.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Eigen>
#include <Eigen/StdVector>


#include <cmath>
#include "stdio.h"
#include <math.h>

// mavros
#include <mavros_msgs/PositionTarget.h>
#include <mavros_msgs/RCIn.h>//遥控器
#include <mavros_msgs/State.h>//状态
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/CommandBool.h>

#define pi 3.14159
#define TAKEOFF_HEIGHT 0.5
#define CAMERA_INT_HEIGHT 0.4

using namespace std;

// 跟踪数据类型
struct TrackingType
{
    std_msgs::Header header;
    geometry_msgs::Point position;
    geometry_msgs::Vector3 velocity;
    double yaw;
    double yaw_rate;
};

// 定位数据类型
struct LocationType
{
    std_msgs::Header header;
    geometry_msgs::Point position;
    geometry_msgs::Quaternion orientation;
};

// 起降数据类型
struct UpDownType
{
    std_msgs::Header header;
    geometry_msgs::Point position;
    double yaw;
};

// 遥控器控制数据类型
struct RemoteType
{
    std_msgs::Header header;
    geometry_msgs::Quaternion orientation;
    geometry_msgs::Vector3 body_rate;
    double thrust;
};

// 控制模式
enum ControllerMode 
{
    TAKEOFF_MODE=0, 
    LAND_MODE, 
    DRIVER_MODE, 
    TRACKING_MODE
};
