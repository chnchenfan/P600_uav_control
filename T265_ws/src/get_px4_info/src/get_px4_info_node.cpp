#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/Range.h>
#include <ros/ros.h>

#include "tf2/LinearMath/Matrix3x3.h"
#include "tf2/LinearMath/Quaternion.h"

#include <string>
#include <fstream>
#include <iostream>

#define RAD2DEG 57.295779513

void odomCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
  // Camera position in map frame
  double tx = msg->pose.pose.position.x;
  double ty = msg->pose.pose.position.y;
  double tz = msg->pose.pose.position.z;

  // Orientation quaternion
  tf2::Quaternion q(msg->pose.pose.orientation.x, msg->pose.pose.orientation.y, msg->pose.pose.orientation.z,
                    msg->pose.pose.orientation.w);

  // 3x3 Rotation matrix from quaternion
  tf2::Matrix3x3 m(q);

  // Roll Pitch and Yaw from rotation matrix
  double roll, pitch, yaw;
  m.getRPY(roll, pitch, yaw);

  // Output the measure
  ROS_INFO("Received odom in '%s' frame : X: %.2f Y: %.2f Z: %.2f - R: %.2f P: %.9f Y: %.2f",
           msg->header.frame_id.c_str(), tx, ty, tz, roll * RAD2DEG, pitch * RAD2DEG, yaw * RAD2DEG);
}

void localPoseCallback(const geometry_msgs::PoseStamped::ConstPtr& msg)
{
  // Camera position in map frame
  double tx = msg->pose.position.x;
  double ty = msg->pose.position.y;
  double tz = msg->pose.position.z;

  // Orientation quaternion
  tf2::Quaternion q(msg->pose.orientation.x, msg->pose.orientation.y, msg->pose.orientation.z, msg->pose.orientation.w);

  // 3x3 Rotation matrix from quaternion
  tf2::Matrix3x3 m(q);

  // Roll Pitch and Yaw from rotation matrix
  double roll, pitch, yaw;
  m.getRPY(roll, pitch, yaw);

  // Output the measure
  ROS_INFO("Received pose in '%s' frame : X: %.2f Y: %.2f Z: %.2f - R: %.2f P: %.2f Y: %.2f",
           msg->header.frame_id.c_str(), tx, ty, tz, roll * RAD2DEG, pitch * RAD2DEG, yaw * RAD2DEG);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "get_px4_info_node");

  ros::NodeHandle n;

  ros::Subscriber sublocalPose = n.subscribe("/mavros/local_position/pose", 10, localPoseCallback);

  ros::spin();

  return 0;
}
