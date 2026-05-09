#include "uav/Driver.h"
//主要是订阅commander的set_activity，position
//说明，设置模式，同时在接受位置信息和
Driver::Driver(ros::NodeHandle& nh,std::string model_name)
{
    int flag;
    nh.getParam("/group_flag", flag);
    yaw_rad=0;
    if(flag==0){
        set_target_position_sub = nh.subscribe("/wjl/set_pose/position",1,&Driver::set_target_position_callback,this);//位置信息设置订阅
        set_target_yaw_sub = nh.subscribe("/wjl/set_pose/orientation",1,&Driver::set_target_yaw_callback,this);//偏航角订阅
        custom_activity_sub = nh.subscribe("/wjl/set_activity/type",1,&Driver::custom_activity_callback,this);//接收到activity后对模式进行设置
        /*订阅*/
        local_pose_sub = nh.subscribe("/mavros/local_position/pose",1,&Driver::local_pose_callback,this);//本地位置订阅
        mavros_sub = nh.subscribe("/mavros/state",1,&Driver::mavros_state_callback,this);//当前模式订阅
        /*发布*/
        local_target_pub = nh.advertise<mavros_msgs::PositionTarget>("/mavros/setpoint_raw/local", 1);//这个话题需要和硬件结合到一起
        /*服务*/
        arming_client = nh.serviceClient<mavros_msgs::CommandBool>("/mavros/cmd/arming");
        set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("/mavros/set_mode");
    }else if(flag==1){
        set_target_position_sub = nh.subscribe("/"+model_name+"/wjl/set_pose/position",1,&Driver::set_target_position_callback,this);//位置信息设置订阅
        set_target_yaw_sub = nh.subscribe("/"+model_name+"/wjl/set_pose/orientation",1,&Driver::set_target_yaw_callback,this);//偏航角订阅
        custom_activity_sub = nh.subscribe("/"+model_name+"/wjl/set_activity/type",1,&Driver::custom_activity_callback,this);//接收到activity后对模式进行设置
        /*订阅*/
        local_pose_sub = nh.subscribe("/"+model_name+"/mavros/local_position/pose",1,&Driver::local_pose_callback,this);//本地位置订阅
        mavros_sub = nh.subscribe("/"+model_name+"/mavros/state",1,&Driver::mavros_state_callback,this);//当前模式订阅
        /*发布*/
        local_target_pub = nh.advertise<mavros_msgs::PositionTarget>("/"+model_name+"/mavros/setpoint_raw/local", 1);//这个话题需要和硬件结合到一起
        /*服务*/
        arming_client = nh.serviceClient<mavros_msgs::CommandBool>("/"+model_name+"/mavros/cmd/arming");
        set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("/"+model_name+"/mavros/set_mode");
    }else{
        ROS_ERROR("参数加载失败，请检查重新启动！！！");
        exit(0);
    }

    set_target_pose(0.0, 0.0, TAKEOFF_HEIGHT, 0.0);
}

Driver::~Driver()
{

}



/*赋值cur_target_pose*/
void Driver::set_target_pose(double x, double y, double z, double yaw)
{
    cur_target_pose.header.stamp = ros::Time::now();
    cur_target_pose.coordinate_frame = 1;
    cur_target_pose.type_mask = 3064;
    cur_target_pose.position.x = x;
    cur_target_pose.position.y = y;
    cur_target_pose.position.z = z;
    cur_target_pose.yaw = yaw;
}

bool Driver::target_frame_valid() const
{
    return cur_target_pose.coordinate_frame == mavros_msgs::PositionTarget::FRAME_LOCAL_NED;
}

void Driver::local_pose_callback(const geometry_msgs::PoseStampedConstPtr& msg)
{
    //ENU坐标系
    //获取无人机当前位置
    cur_position_x = msg->pose.position.x;
    cur_position_y = msg->pose.position.y;
    cur_position_z = msg->pose.position.z;
}



void Driver::mavros_state_callback(const mavros_msgs::StateConstPtr& msg)
{
    mavros_state = *msg;
}


void Driver::set_target_position_callback(const geometry_msgs::PoseStampedConstPtr& msg)
{
    //基于世界坐标系直接发送位置信息
    set_target_pose(msg->pose.position.x, msg->pose.position.y ,msg->pose.position.z, yaw_rad);
}
//设置期望角度，只有接收到才有，否则为0
void Driver::set_target_yaw_callback(const std_msgs::Float64ConstPtr& msg)
{
    yaw_rad =  msg->data*pi/180;
    set_target_pose(cur_target_pose.position.x,cur_target_pose.position.y,cur_target_pose.position.z,yaw_rad);
}

void Driver::custom_activity_callback(const std_msgs::StringConstPtr& msg)
{ 
    if(msg->data == "OFFBOARD")
    {
        offboard();
    }
    if(msg->data == "ARM")
    {
        arm();
    }
    if(msg->data == "DISARM")
    {
        disarm();
    }
    if(msg->data == "LAND")
    {
        land();
    }
    if(msg->data == "HOVER")
    {
        hover();
    }
    if(msg->data == "TAKEOFF")
    {
        takeoff();
    }
}

void Driver::land()
{
    set_mode.request.custom_mode = "AUTO.LAND";
    if (!set_mode_client.call(set_mode)) {
        ROS_ERROR("LAND mode request failed: service call error");
        return;
    }
    if (!set_mode.response.mode_sent) {
        ROS_WARN("LAND mode request was sent but PX4 did not accept AUTO.LAND");
        return;
    }
    ROS_INFO("AUTO.LAND request accepted");
}

void Driver::offboard()
{
    // cout<<"offboardingng!!!"<<endl;
    set_mode.request.custom_mode = "OFFBOARD";
    set_mode_client.call(set_mode);  
}

void Driver::arm()
{
    if(!arm_cmd.response.success)
    {
        // cout<<"arming!!!"<<endl;
        arm_cmd.request.value = true;
        arming_client.call(arm_cmd);      
    }

}

void Driver::disarm()
{
    arm_cmd.request.value = false;
    arming_client.call(arm_cmd);
}

void Driver::hover()
{
  
}

void Driver::takeoff()
{
    if(!arm_offb_detection())
    {
        set_target_pose(0,0,TAKEOFF_HEIGHT,0);

        if(!target_frame_valid()) {
            ROS_ERROR("TAKEOFF blocked: invalid setpoint coordinate_frame=%u, expected FRAME_LOCAL_NED(1)",
                      static_cast<unsigned>(cur_target_pose.coordinate_frame));
            return;
        }

        offboard();
        arm();
        cout<<"起飞！"<<endl;
    }
}

bool Driver::arm_offb_detection()
{
    if(mavros_state.mode == "OFFBOARD" && mavros_state.armed)
        return true;
    else
        return false;
}

void Driver::start()
{
    ros::Rate loop_rate(30);
    while(ros::ok())
    {
        // 进入 AUTO.LAND 后停止继续刷 OFFBOARD 位置 setpoint，避免与 PX4 的降落模式冲突。
        if(mavros_state.mode != "AUTO.LAND") {
            if(!target_frame_valid()) {
                ROS_ERROR_THROTTLE(1.0,
                    "Invalid setpoint coordinate_frame=%u, expected FRAME_LOCAL_NED(1)",
                    static_cast<unsigned>(cur_target_pose.coordinate_frame));
            }
            local_target_pub.publish(cur_target_pose);//这里一直发布期望位置
        }
        ros::spinOnce();
        loop_rate.sleep();
        if(mavros_state.mode == "AUTO.LAND" && cur_position_z < 0.1)
        disarm();
    }
}

