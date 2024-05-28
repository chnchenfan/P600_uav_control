#ifndef ODOMETRY_H
#define ODOMETRY_H


#include<ros/ros.h>
#include "odometry.h"
#include<nav_msgs/Odometry.h>
#include<nlink_parser/LinktrackNodeframe2.h>
#include<sensor_msgs/Range.h>


class odom_message
{ 
public:
    odom_message(ros::NodeHandle& nh);
    void uwb_callback(const nlink_parser::LinktrackNodeframe2ConstPtr& uwb_msg);
    void tf_callback(const sensor_msgs::RangeConstPtr& tf_msg);
private:
    ros::NodeHandle nh; 
    nav_msgs::Odometry uwb_pos;
    ros::Subscriber uwb_sub;
    ros::Subscriber tf_sub;
    ros::Publisher uwb_pub;
    double z;
};
#endif

