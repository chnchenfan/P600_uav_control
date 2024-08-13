#include "realsense/px4_realsense_bridge.h"
std::string portName;
int baud_rate;
float dist;
mySerial::mySerial *serial_obj;
namespace bridge {

PX4_Realsense_Bridge::PX4_Realsense_Bridge(const ros::NodeHandle& nh,std::string model_name): nh_(nh) {
    odom_sub_ = nh_.subscribe<const nav_msgs::Odometry&>("/camera/odom/sample_throttled", 10, &PX4_Realsense_Bridge::odomCallback, this);
    // publishers
    int flag;
    nh_.getParam("/group_flag", flag);
    if(flag == 0){
        mavros_odom_pub_ = nh_.advertise<nav_msgs::Odometry>("/mavros/odometry/out", 10);
        mavros_system_status_pub_ = nh_.advertise<mavros_msgs::CompanionProcessStatus>("/mavros/companion_process/status", 1);
    }else if(flag==1){
        mavros_odom_pub_ = nh_.advertise<nav_msgs::Odometry>("/"+model_name+"/mavros/odometry/out", 10);
        mavros_system_status_pub_ = nh_.advertise<mavros_msgs::CompanionProcessStatus>("/"+model_name+"/mavros/companion_process/status", 1);
    }else{
        ROS_ERROR("参数加载失败，请检查重新启动！！！");
        exit(0);
    }
    last_callback_time = ros::Time::now();
    status_mutex_.reset(new std::mutex);
    worker_ = std::thread(&PX4_Realsense_Bridge::publishSystemStatus, this);
    nh.param("/tof_usb", portName, std::string("/dev/tof"));//具体的哪一个串口
    nh.param("/baud_tof_rate", baud_rate, 115200);//该串口的波特率
    serial_obj = new mySerial::mySerial(portName, baud_rate); //以上参数赋值给定义的串口对象

};

PX4_Realsense_Bridge::~PX4_Realsense_Bridge() { }


void PX4_Realsense_Bridge::odomCallback(const nav_msgs::Odometry& msg) {

    // publish odometry msg
    // nav_msgs::Odometry output = msg;
    output.header = msg.header;
    output.twist = msg.twist;
    output.pose.covariance = msg.pose.covariance;
    output.pose.pose.orientation = msg.pose.pose.orientation;
    output.pose.pose.position.x = msg.pose.pose.position.x;
    output.pose.pose.position.y = msg.pose.pose.position.y;
    output.pose.pose.position.z = dist;
    output.header.frame_id = msg.header.frame_id;
    output.child_frame_id = msg.child_frame_id;
    mavros_odom_pub_.publish(output);

    flag_first_pose_received = true;

    { // lock mutex
        std::lock_guard<std::mutex> status_guard(*(status_mutex_));

        last_system_status_ = system_status_;

        // check confidence in vision estimate by looking at covariance
        if( msg.pose.covariance[0] > 0.1 ) // low confidence -> reboot companion
        {
        system_status_ = MAV_STATE::MAV_STATE_FLIGHT_TERMINATION;
        }
        else if( msg.pose.covariance[0] == 0.1 ) // medium confidence
        {
        system_status_ = MAV_STATE::MAV_STATE_CRITICAL;
        }
        else if( msg.pose.covariance[0] == 0.01 ) // high confidence
        {
        system_status_ = MAV_STATE::MAV_STATE_ACTIVE;
        }
        else
        {
        ROS_WARN_STREAM("Unexpected vision sensor variance");
        }  

        // publish system status immediately if it changed
        if( last_system_status_ != system_status_ )
        {
        mavros_msgs::CompanionProcessStatus status_msg;
        status_msg.header.stamp = ros::Time::now();
        status_msg.component = 197;  // MAV_COMP_ID_VISUAL_INERTIAL_ODOMETRY
        status_msg.state = (int)system_status_;
        mavros_system_status_pub_.publish(status_msg);
        }  

    last_callback_time = ros::Time::now();
        
    }
}


void PX4_Realsense_Bridge::publishSystemStatus(){
    while(ros::ok()){
        ros::Duration(0.02).sleep();
        if(flag_first_pose_received == true) { // only send heartbeat if we receive pose estimates at all
        // check if we received an recent update
        // otherwise let the companion computer restart
        if( (ros::Time::now()-last_callback_time) > ros::Duration(0.5) ){
            ROS_WARN_STREAM("Stopped receiving data from T265");
            system_status_ = MAV_STATE::MAV_STATE_FLIGHT_TERMINATION;
        }
        mavros_msgs::CompanionProcessStatus status_msg;
        status_msg.header.stamp = ros::Time::now();
        status_msg.component = 197;  // MAV_COMP_ID_VISUAL_INERTIAL_ODOMETRY
        dist = serial_obj->getDist();
        ROS_INFO("%.3f",dist);
        { // lock mutex
            std::lock_guard<std::mutex> status_guard(*(status_mutex_));
            status_msg.state = (int)system_status_;
            mavros_system_status_pub_.publish(status_msg);
        }
        }
    }

}

}
