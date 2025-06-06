#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <nav_msgs/Odometry.h>
#include <Eigen/Eigen>
 
 

 geometry_msgs::PoseStamped vrpn_data;
 
void vrpn_callback(const geometry_msgs::PoseStamped::ConstPtr &msg)
{
    if(msg->header.frame_id == "world")
    {
        vrpn_data = *msg;
    }
}
 
 
int main(int argc, char **argv)
{
    ros::init(argc, argv, "vrpn_to_mavros");
    ros::NodeHandle nh("~");
 
    ros::Subscriber slam_sub = nh.subscribe<geometry_msgs::PoseStamped>("odom", 100, vrpn_callback);
 
    ros::Publisher vision_pub = nh.advertise<geometry_msgs::PoseStamped>("vision_pose", 10);
 
 
    // the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(50.0);
 
    ros::Time last_request = ros::Time::now();
 
    while(ros::ok()) {
        geometry_msgs::PoseStamped vision;
 
        // vision.pose.position.x = p_mav[0];
        // vision.pose.position.y = p_mav[1];
        // vision.pose.position.z = p_mav[2];
 
        // vision.pose.orientation.x = q_mav.x();
        // vision.pose.orientation.y = q_mav.y();
        // vision.pose.orientation.z = q_mav.z();
        // vision.pose.orientation.w = q_mav.w();

        vision = vrpn_data;
 
        vision.header.stamp = ros::Time::now();
        vision_pub.publish(vision);
 
        // ROS_INFO("z is %.4f",vision.pose.position.z);
 
        ros::spinOnce();
        rate.sleep();
    }
 
    return 0;
}

