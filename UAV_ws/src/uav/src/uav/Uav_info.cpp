#include "Uav_info.h"

Uav_info::Uav_info(ros::NodeHandle& nh,std::string model_name):cmd(nh,model_name)
{
    setpoint_pos.x=0;
    setpoint_pos.y=0;
    setpoint_pos.z=0.5;
    RC_control = false;
    this->model_name=model_name;
    int flag;
    nh.getParam("/group_flag", flag);
    if(flag==0){
        rc_sub = nh.subscribe<mavros_msgs::RCIn>("/mavros/rc/in",10,&Uav_info::RC_callback, this);
        local_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/mavros/local_position/pose",10,&Uav_info::mr_localpose_callback, this);
        mavros_state_sub = nh.subscribe<mavros_msgs::State>("/mavros/state",10,&Uav_info::mr_state_callback, this);
    }else if(flag==1){
        rc_sub = nh.subscribe<mavros_msgs::RCIn>(model_name+"/mavros/rc/in",10,&Uav_info::RC_callback, this);
        local_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>(model_name+"/mavros/local_position/pose",10,&Uav_info::mr_localpose_callback, this);
        mavros_state_sub = nh.subscribe<mavros_msgs::State>(model_name+"/mavros/state",10,&Uav_info::mr_state_callback, this);
    }else{
        ROS_INFO("参数加载失败！！！");
    }
    
}

Uav_info::~Uav_info()
{
}


bool Uav_info::isArrived()
{
    targetPos << setpoint_pos.x, setpoint_pos.y, setpoint_pos.z;
    biasPos = targetPos - currentPos;
    cout<<"距离目标点的距离为 ="<< biasPos.norm() <<endl;
    if(biasPos.norm()<0.2)
    {
        ros::Duration(1).sleep();
        if(biasPos.norm()<0.2)
        {
            return true;
        }
        else
        {
            return false;
        }      
    }
    else
    {
        return false;
    }
}
void Uav_info::IsArrived(double x,double y,double z){
    ros::Rate r(30);
    Set_pose(x,y,z);
    while(ros::ok()){
        if(isArrived())
        {
            ROS_INFO("炸鸡已到达上方");
            ros::Duration(3).sleep();
            break;
        }
        else
        {
            Set_pose(x,y,z);
            std::cout<<"正在前往x:"<<x<<" y:"<<y<<" z:"<<z<<std::endl;
        }
        r.sleep();
        ros::spinOnce();    
    }
}
void Uav_info::Set_arm_offboard(){
    ros::Rate loop_rate(30);
    while(ros::ok() && !current_mavros_state.armed)
    {
        cmd.arm();
        loop_rate.sleep();     
        ros::spinOnce();
    }
    ROS_INFO("解锁成功");
    ROS_INFO("即将进入offboard");
    while(ros::ok() && current_mavros_state.mode != "OFFBOARD")
    {
        cmd.move(0, 0,setpoint_pos.z,"ENU");
        cmd.offboard();
        ros::spinOnce();
    }
    ROS_INFO("进入offboard成功");
}
void Uav_info::Set_pose(double x,double y,double z){
    std::cout<<"设置期望的目标点为："<<x<<" "<<y<<" "<<z<<std::endl;
    setpoint_pos.x=x;
    setpoint_pos.y=y;
    setpoint_pos.z=z;
    this->cmd.move(x,y,z);
}

void Uav_info::mr_localpose_callback(const geometry_msgs::PoseStampedConstPtr& localposemsg)
{
  //ENU坐标系
  //获取无人机当前位置
  cur_position_x = localposemsg->pose.position.x;
  cur_position_y = localposemsg->pose.position.y;
  cur_position_z = localposemsg->pose.position.z;
  currentPos<< cur_position_x, cur_position_y, cur_position_z;
}

void Uav_info::mr_state_callback(const mavros_msgs::State::ConstPtr& mavrosstatemsg)
{
    current_mavros_state = *mavrosstatemsg;
}



void Uav_info::RC_callback(const mavros_msgs::RCIn::ConstPtr& RCmsg)
{
//   这里的RCmsg->channels.at(6) 中的6是对应的通道7（SWA），往下按生效
    if(RCmsg->channels.at(6) >1500)
    {
        RC_control = true;
    }
    else
    {
        RC_control = false;
    }
}

