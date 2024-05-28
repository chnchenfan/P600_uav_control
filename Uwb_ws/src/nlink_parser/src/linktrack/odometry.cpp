#include<iostream>
#include<ros/ros.h>
#include "odometry.h"
#include<nav_msgs/Odometry.h>
#include<nlink_parser/LinktrackNodeframe2.h>
#include "nlink_unpack/nlink_linktrack_nodeframe2.h"
#include<sensor_msgs/Range.h>



odom_message::odom_message(ros::NodeHandle& nh)
{
    tf_sub = nh.subscribe<sensor_msgs::Range>("/tfmini_ros_node/TFmini", 10, &odom_message::tf_callback,this);
    uwb_sub = nh.subscribe<nlink_parser::LinktrackNodeframe2>("/nlink_linktrack_nodeframe2", 10, &odom_message::uwb_callback,this);
    odom_message::uwb_pub = nh.advertise<nav_msgs::Odometry>("/uwb/odom/sample", 100);
    z=0;
}

void odom_message::tf_callback(const sensor_msgs::RangeConstPtr& tf_msg)
{
    odom_message::uwb_pos.pose.pose.position.z=tf_msg->range;
}

void odom_message::uwb_callback(const nlink_parser::LinktrackNodeframe2ConstPtr& uwb_msg)
{
    odom_message::uwb_pos.pose.pose.position.x=uwb_msg->pos_3d[0];
    odom_message::uwb_pos.pose.pose.position.y=uwb_msg->pos_3d[1];
    //    odom_message::uwb_pos.pose.pose.position.z=odom_message::z;
        // odom_message::uwb_pos.pose.pose.position.z=uwb_msg->pos_3d[2];
    odom_message::uwb_pos.header.stamp=ros::Time::now();
        //odom_message::uwb_pos.pose.pose.orientation.x = uwb_msg->quaternion[1];
        ///odom_message::uwb_pos.pose.pose.orientation.y = uwb_msg->quaternion[2];
        //odom_message::uwb_pos.pose.pose.orientation.z = uwb_msg->quaternion[3];
        //odom_message::uwb_pos.pose.pose.orientation.w = uwb_msg->quaternion[0];
    odom_message::uwb_pos.header.frame_id="uwb_odom_frame";
      //  odom_message::uwb_pos.child_frame_id="base_link";
    uwb_pub.publish(uwb_pos);
       // ros::spinOnce();
}

int main(int argc,char *argv[])
{
    ros::init(argc, argv, "odom_node"); //ros系统的初始化，最后一个参数为节点名称
    ros::NodeHandle nh; 

    odom_message odom_init(nh);
    ros::Rate rate(30.0);
    ROS_INFO("start position!!!!!!!!!");
    while(ros::ok())
    {
        ros::spinOnce();
	    rate.sleep();
    }
    return 0;

}
