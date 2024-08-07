#include "Driver.h"
//主要是订阅commander的set_activity，position
//说明，设置模式，同时在接受位置信息和
Driver::Driver(ros::NodeHandle& nh,std::string model_name)
{
    set_target_position_sub = nh.subscribe("/wjl/set_pose/position",1,&Driver::set_target_position_callback,this);//位置信息设置订阅
    set_target_yaw_sub = nh.subscribe("/wjl/set_pose/orientation",1,&Driver::set_target_yaw_callback,this);//偏航角订阅
    custom_activity_sub = nh.subscribe("/wjl/set_activity/type",1,&Driver::custom_activity_callback,this);//接收到activity后对模式进行设置

    int flag;
    nh.getParam("group_flag", flag);
    if(flag==0){
        /*订阅*/
        local_pose_sub = nh.subscribe("/mavros/local_position/pose",1,&Driver::local_pose_callback,this);//本地位置订阅
        mavros_sub = nh.subscribe("/mavros/state",1,&Driver::mavros_state_callback,this);//当前模式订阅
        /*发布*/
        local_target_pub = nh.advertise<mavros_msgs::PositionTarget>("/mavros/setpoint_raw/local", 1);//这个话题需要和硬件结合到一起
        /*服务*/
        arming_client = nh.serviceClient<mavros_msgs::CommandBool>("/mavros/cmd/arming");
        set_mode_client = nh.serviceClient<mavros_msgs::SetMode>("/mavros/set_mode");
    }else if(flag==1){
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
}

Driver::~Driver()
{

}



/*赋值cur_target_pose*/
void Driver::set_target_pose(double x, double y, double z, double yaw)
{
    cur_target_pose.header.stamp = ros::Time::now();
    cur_target_pose.coordinate_frame = 1;//ENU_LOCAL
    cur_target_pose.type_mask = 1016;
    cur_target_pose.position.x = x;
    cur_target_pose.position.y = y;
    cur_target_pose.position.z = z;
    cur_target_pose.yaw = yaw - (20*pi/180);
    // cout<<"yaw: "<<yaw<<endl;
    cur_target_pose.yaw_rate = 1;
}

void Driver::local_pose_callback(const geometry_msgs::PoseStampedConstPtr& msg)
{
    //ENU坐标系
    //获取无人机当前位置
    cur_position_x = msg->pose.position.x;
    cur_position_y = msg->pose.position.y;
    cur_position_z = msg->pose.position.z;
    // std::cout<<cur_position_x<<"  "<<cur_position_y<<" "<<cur_position_z<<endl;
    //获取无人机当前姿态（四元数）
    double x = msg->pose.orientation.x;
    double y = msg->pose.orientation.y;
    double z = msg->pose.orientation.z;
    double w = msg->pose.orientation.w;
    Quaternion2yaw(x,y,z,w);
    //cout<<cur_heading_rad*180/pi<<endl;
}

/*机体坐标系到全局坐标系转换,只考虑yaw,忽略pitch和roll,其中yaw为弧度值*/
// 就是绕着z轴旋转后的坐标系之间的关系，看图即可
vector<double> Driver::FLU2ENU(double x_flu, double y_flu, double z_flu)
{
    vector<double> position_enu;
    double x_enu = x_flu*cos(cur_heading_rad)-y_flu*sin(cur_heading_rad);
    double y_enu = x_flu*sin(cur_heading_rad)+y_flu*cos(cur_heading_rad);
    double z_enu = z_flu;
    position_enu.push_back(x_enu);
    position_enu.push_back(y_enu);
    position_enu.push_back(z_enu);
    return position_enu;
}
//这是根据四元数求解偏航角计算
void Driver::Quaternion2yaw(double x, double y, double z, double w)
{
    Eigen::Quaterniond q;
    q.x() = x;
    q.y() = y;
    q.z() = z;
    q.w() = w;
    double siny_cosp = +2.0 * (q.w() * q.z() + q.x() * q.y());
    double cosy_cosp = +1.0 - 2.0 * (q.y() * q.y() + q.z() * q.z());
    cur_heading_rad = atan2(siny_cosp, cosy_cosp);

}

void Driver::mavros_state_callback(const mavros_msgs::StateConstPtr& msg)
{
    mavros_state = *msg;
    //cout<<mavros_state<<endl;
}


void Driver::set_target_position_callback(const geometry_msgs::PoseStampedConstPtr& msg)
{
    double enu_x, enu_y, enu_z;
    if(msg->header.frame_id == "base_link")
    {
        //基于当前的机体坐标系发送位置信息
        double flu_x = msg->pose.position.x;
        double flu_y = msg->pose.position.y;
        double flu_z = msg->pose.position.z;
        vector<double> offset_enu;
        offset_enu = FLU2ENU(flu_x, flu_y, flu_z);

        double offset_enu_x = offset_enu[0];
        double offset_enu_y = offset_enu[1];
        double offset_enu_z = offset_enu[2];
        
        enu_x = offset_enu_x + cur_position_x;
        enu_y = offset_enu_y + cur_position_y;
        enu_z = offset_enu_z + cur_position_z;
    }
    else
    {
        //基于世界坐标系直接发送位置信息
        enu_x = msg->pose.position.x;
        enu_y = msg->pose.position.y;
        enu_z = msg->pose.position.z;
    }

    set_target_pose(enu_x, enu_y ,enu_z, yaw_rad);
}
//设置期望角度，只有接收到才有，否则为0
void Driver::set_target_yaw_callback(const std_msgs::Float64ConstPtr& msg)
{
    yaw_rad = cur_heading_rad + msg->data*pi/180;
    set_target_pose(cur_target_pose.position.x,cur_target_pose.position.y,TAKEOFF_HEIGHT,yaw_rad);
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
    set_mode_client.call(set_mode);
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
        //set_target_pose(cur_position_x,cur_position_y,TAKEOFF_HEIGHT,cur_heading_rad);
        set_target_pose(0,0,TAKEOFF_HEIGHT,0);
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
    ros::Rate loop_rate(20);
    while(ros::ok())
    {
        local_target_pub.publish(cur_target_pose);//这里一直发布期望位置
        ros::spinOnce();
        loop_rate.sleep();
        if(mavros_state.mode == "AUTO.LAND" && cur_position_z < 0.1)
        disarm();
    }
}

