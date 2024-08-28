#include <ros/ros.h>
#include <gazebo_msgs/ModelStates.h>
#include <gazebo_msgs/LinkStates.h>
#include <geometry_msgs/Pose.h>
#include <tf/transform_datatypes.h>
#include <geometry_msgs/PoseStamped.h>
#include </usr/include/eigen3/Eigen/Dense>
#include "other/Print_.h"
geometry_msgs::PoseStamped base_link_pose;//存储base_link相对于世界坐标的信息
bool is_updated = false;
bool flag_1=true,flag_2=true;
int id1=-1;
std::string gazebo_model_name;
void Judge_param_load(ros::NodeHandle nh);//判断参数是否加载成功
void modelStatesCallback(const gazebo_msgs::ModelStates::ConstPtr& msg);

int main(int argc, char** argv) {
    setlocale(LC_ALL, "");
    ros::init(argc, argv, "gazebo_pose0");
    ros::NodeHandle nh;
    Judge_param_load(nh);
    int flag;
    std::string model_name;
    nh.getParam("/group_flag", flag);
    nh.getParam("/model_name0", model_name); 
    gazebo_model_name=model_name;
    ros::Subscriber sub;
    ros::Publisher pose_pub;
    if(flag==0){
        sub = nh.subscribe("/gazebo/model_states", 10, modelStatesCallback);
        pose_pub=nh.advertise<geometry_msgs::PoseStamped>("/wjl/gazebo_real/base_link_pose",10);
    }else if(flag==1){
        sub = nh.subscribe("/gazebo/model_states", 10, modelStatesCallback);
        pose_pub=nh.advertise<geometry_msgs::PoseStamped>("/"+model_name+"/wjl/gazebo_real/base_link_pose",10);
    }else{
        ROS_ERROR("参数加载失败，请检查重新启动！！！");
        exit(0);
    }
    Print_green("gazebo_pose0节点启动!!!");
    ros::Rate r(30);
    while(ros::ok()){
        pose_pub.publish(base_link_pose);
        ros::spinOnce();
        r.sleep();
    }
    return 0;
}

void Judge_param_load(ros::NodeHandle nh){
    while (!nh.hasParam("/load_param_flag"))
  	{
    	ROS_INFO("等待参数加载...");
    	ros::Duration(1.0).sleep();
  	}
    ROS_INFO("yaml文件参数加载成功");
}
//获取gazebo仿真两个模型的信息以及位置关系
void modelStatesCallback(const gazebo_msgs::ModelStates::ConstPtr& msg) {
    std::vector<std::string> model_names = msg->name;
    std::vector<geometry_msgs::Pose> model_poses = msg->pose;
    //第一次判断物体得到序号
    if(flag_1){
    //因为sdf文件可能加入其他模型，序号可能变，所以需要判断
    //判断模型的序号，检测用
        for (size_t i = 0; i < model_names.size(); ++i) {
            // std::cout<<i<<" "<<model_names[i]<<std::endl;
            if (model_names[i] == gazebo_model_name) {
                id1 = i;
            }
        }
        flag_1=false;
    }
    geometry_msgs::Pose pose;
    pose=model_poses[id1];
    base_link_pose.header.stamp = ros::Time::now();
    base_link_pose.header.frame_id = "world";
    base_link_pose.pose = pose;
}
