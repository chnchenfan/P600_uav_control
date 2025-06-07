#include "uav/Uav_info.h"
Uav_info::Uav_info(ros::NodeHandle& nh,std::string model_name):cmd(nh,model_name)
{
    setpoint_pos.x=0;
    setpoint_pos.y=0;
    setpoint_pos.z=0.5;
    this->model_name=model_name;
    int flag;
    guidefly_flag=false;
    yaw_d=0;
    land_flag=false;
    nh.getParam("/group_flag", flag);
    uav_guidefly_sub = nh.subscribe<uav::xyz_yaw_d>("/wjl/guidefly/pose_d",10,&Uav_info::Uav_guidefly_cb, this);
    if(flag==0){
        local_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/mavros/local_position/pose",10,&Uav_info::mr_localpose_callback, this);
        mavros_state_sub = nh.subscribe<mavros_msgs::State>("/mavros/state",10,&Uav_info::mr_state_callback, this);
    }else if(flag==1){
        local_pose_sub = nh.subscribe<geometry_msgs::PoseStamped>("/"+model_name+"/mavros/local_position/pose",10,&Uav_info::mr_localpose_callback, this);
        mavros_state_sub = nh.subscribe<mavros_msgs::State>("/"+model_name+"/mavros/state",10,&Uav_info::mr_state_callback, this);
    }else{
        ROS_ERROR("参数加载失败，请检查重新启动！！！");
        exit(0);
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
    if(biasPos.norm()<0.05)
    {
        ros::Duration(1).sleep();
        if(biasPos.norm()<0.05)
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
            ros::Duration(1).sleep();
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
        cmd.move(0, 0,setpoint_pos.z);
        cmd.offboard();
        ros::spinOnce();
    }
    ROS_INFO("进入offboard成功");
}
void Uav_info::Set_pose(double x,double y,double z){
    // std::cout<<"设置期望的目标点为："<<x<<" "<<y<<" "<<z<<std::endl;
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


void Uav_info::Uav_guidefly_cb(const boost::shared_ptr<const uav::xyz_yaw_d>& msg){
    // std::cout<<"指点飞行:x"<<msg->x_d<<" y:"<<msg->y_d<<" z:"<<msg->z_d<<std::endl;
    Set_pose(msg->x_d,msg->y_d,msg->z_d);
    this->cmd.turn(msg->yaw_d);
    yaw_d=msg->yaw_d;
    land_flag=msg->land_flag;
    guidefly_flag=true;
}
