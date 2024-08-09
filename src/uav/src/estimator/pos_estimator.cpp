#include "estimator/pos_estimator.h"
Pos_estimator::Pos_estimator(ros::NodeHandle& nh,std::string model_name){
    int flag;
    nh.getParam("/group_flag", flag);
    nh.getParam("/estimator_num", estimator_num);
    if(flag==0){
        std::cout<<"未使用组名"<<std::endl;
        vision_pose_pub = nh.advertise<geometry_msgs::PoseStamped>("/mavros/vision_pose/pose", 10);
        gazebo_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/gazebo_real/pose", 10,&Pos_estimator::gazebo_cb, this);
    }else if(flag==1){
        std::cout<<"使用了组名,组名为:"<<model_name<<std::endl;
        vision_pose_pub = nh.advertise<geometry_msgs::PoseStamped>("/"+model_name+"/mavros/vision_pose/pose", 10);
    }else{
        ROS_ERROR("参数加载失败，请检查重新启动！！！");
        exit(0);
    }
    if(estimator_num==1){
        std::cout<< "\033选择gazebo真值信息定位!!!\033[0m"<< endl;
    }
}
void Pos_estimator::run(){
    ros::Rate r(20);
    while(ros::ok){
        if(estimator_num==1){
            vision_pose_pub.publish(vision_pose);
        }else{

        }
        r.sleep();
        ros::spinOnce;
    }
}
void Pos_estimator::gazebo_cb(const geometry_msgs::PoseStampedConstPtr& msg){
    vision_pose=*msg;
}