#include <ros/ros.h>
#include <dynamic_reconfigure/server.h>
#include <uav/uav_cmdConfig.h>
#include "std_msgs/Float64.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <geometry_msgs/Point.h>
#include <std_msgs/Bool.h>
#include "uav/xyz_yaw_d.h"
class All_joints{
public:
    uav::uav_cmdConfig config;
    ros::NodeHandle nh;
    ros::Publisher uav_pos_d_pub;
    uav::xyz_yaw_d uav_pos_d;
    All_joints(ros::NodeHandle nh)
    {
        this->nh=nh;
        this->uav_pos_d_pub=nh.advertise<uav::xyz_yaw_d>("/wjl/guidefly/pose_d",10);
    }
    void Pub(){
        uav_pos_d.x_d=config.uav_x_d;
        uav_pos_d.y_d=config.uav_y_d;
        uav_pos_d.z_d=config.uav_z_d;
        uav_pos_d.yaw_d=config.uav_yaw_d;
        uav_pos_d.land_flag=config.land_flag;
        //保证话题发布成功
        ros::Rate rate(60);
        int count=0;
        while(ros::ok()){
            uav_pos_d_pub.publish(uav_pos_d);
            rate.sleep();
            ros::spinOnce();
            if(count>20){
                break;
            }
            count++;
        }
    }
};

void callback(uav::uav_cmdConfig &config, uint32_t level,All_joints& All_joints) {
    All_joints.config=config;
    All_joints.Pub();
}
 
int main(int argc,char* argv[])
{
    setlocale(LC_ALL,"");
    ros::init(argc,argv,"dynamic_guidefly");
    ros::NodeHandle nh;
    All_joints all_joints(nh);
    dynamic_reconfigure::Server<uav::uav_cmdConfig> server;
    dynamic_reconfigure::Server<uav::uav_cmdConfig>::CallbackType f;
    f = boost::bind(&callback, _1, _2,boost::ref(all_joints));
    server.setCallback(f);
    ROS_INFO("dynamic_angle is ready");
    ros::spin();
}