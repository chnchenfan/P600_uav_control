#include "Uav_info.h"

Uav_info::Uav_info(ros::NodeHandle& nh,std::string model_name):cmd(nh,model_name)
{

    setpoint_pos.x=0;
    setpoint_pos.y=0;
    setpoint_pos.z=1;
    RC_control = true;
    rc_sub = nh.subscribe<mavros_msgs::RCIn>("/mavros/rc/in",10,&Uav_info::RC_callback, this);
    this->model_name=model_name;
    local_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/mavros/local_position/pose",10,&Uav_info::mr_localpose_callback, this);
    mavros_state_sub = nh.subscribe<mavros_msgs::State>("/mavros/state",10,&Uav_info::mr_state_callback, this);
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
        // ROS_INFO("333");
        cmd.move(0, 0,setpoint_pos.z,"ENU");
        cmd.offboard();
        ros::spinOnce();
    }
    ROS_INFO("进入offboard成功");
}
void Uav_info::Set_pose(double x,double y,double z){
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
//   这里的RCmsg->channels.at(9) 中的9是对应的通道8，往下按生效
    if(RCmsg->channels.at(9) >1500)
    {
        RC_control = true;
    }
    else
    {
        RC_control = false;
    }
}

