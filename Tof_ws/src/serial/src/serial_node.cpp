#include <mySerial.h>
#include <ros/ros.h>
#include <sensor_msgs/Range.h>
#include <iostream>

int main(int argc, char **argv)
{
  ros::init(argc, argv, "tfmini_ros_node");
  ros::NodeHandle nh;
  ros::Publisher pub_range = nh.advertise<sensor_msgs::Range>("tfmini_ros_node/TFmini", 1, true);
  sensor_msgs::Range tfmini_range;
  tfmini_range.radiation_type = sensor_msgs::Range::INFRARED;
  tfmini_range.field_of_view = 0.04;
  tfmini_range.min_range = 0.1;
  tfmini_range.max_range = 12;
  tfmini_range.header.frame_id = "tfmini";

  std::string portName;
  int baud_rate;
  mySerial::mySerial *serial_obj;
  nh.param("serial_port", portName, std::string("/dev/ttyUSB0"));//具体的哪一个串口
  nh.param("baud_rate", baud_rate, 115200);//该串口的波特率
  serial_obj = new mySerial::mySerial(portName, baud_rate); //以上参数赋值给定义的串口对象

  ROS_INFO_STREAM("Start processing ...");

  ros::Rate loop_rate(50);
  float dist = 0;

  while(ros::master::check() && ros::ok())
  {
    dist = serial_obj->getDist();
    if(dist > 0 && dist < tfmini_range.max_range)
    {
      tfmini_range.range = dist;
      tfmini_range.header.stamp = ros::Time::now();
      pub_range.publish(tfmini_range);
    }
    else
    {
      ROS_ERROR_STREAM("Data validation error!");
    }
    ros::spinOnce();
    loop_rate.sleep();
  }

  serial_obj->closePort();

  return 0;
}

