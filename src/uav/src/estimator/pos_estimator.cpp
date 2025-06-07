#include "estimator/pos_estimator.h"
Pos_estimator::Pos_estimator(ros::NodeHandle& nh,std::string model_name){
    int flag;
    nh.getParam("/group_flag", flag);
    nh.getParam("/estimator_flag", estimator_flag);
    std::string mocap_object;
    nh.getParam("/mocap_object", mocap_object);
    // mocap_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/vrpn_client_node/"+ mocap_object + "/pose", 30,&Pos_estimator::mocap_cb, this);
    if(flag==0){
        Print_green("未使用组名");
        vision_pose_pub = nh.advertise<geometry_msgs::PoseStamped>("/mavros/vision_pose/pose", 30);
        gazebo_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/wjl/gazebo_real/base_link_pose", 10,&Pos_estimator::gazebo_cb, this);
    }else if(flag==1){
        Print_green("使用了组名，组名："+model_name);
        vision_pose_pub = nh.advertise<geometry_msgs::PoseStamped>("/"+model_name+"/mavros/vision_pose/pose", 30);
        gazebo_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/"+model_name+"/wjl/gazebo_real/base_link_pose", 30,&Pos_estimator::gazebo_cb, this);
    }else{
        ROS_ERROR("参数加载失败，请检查重新启动！！！");
        exit(0);
    }
    if(estimator_flag=="w1"){
        Print_green("选择gazebo真实位置信息定位!!!");
    }else if(estimator_flag=="w3"){
        Print_green("选择动捕信息定位!!!");
    }
}
void Pos_estimator::run(){
    ros::Rate r(30);
    while(ros::ok){
        if(estimator_flag=="w1"){
            vision_pose=gazebo_pose;
            vision_pose_pub.publish(vision_pose);
        }
        r.sleep();
        ros::spinOnce();
    }
}
void Pos_estimator::mocap_cb(const geometry_msgs::PoseStampedConstPtr& msg){
    mocap_pose=*msg;
}

void Pos_estimator::gazebo_cb(const geometry_msgs::PoseStampedConstPtr& msg){
    gazebo_pose=*msg;
}