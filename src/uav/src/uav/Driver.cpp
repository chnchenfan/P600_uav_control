#include "uav/Driver.h"
//主要是订阅commander的set_activity，position
//说明，设置模式，同时在接受位置信息和
Driver::Driver(ros::NodeHandle& nh,std::string model_name)
{
    int flag;
    nh.getParam("/group_flag", flag);
    yaw_rad=0;
    cur_position_x = 0.0;
    cur_position_y = 0.0;
    cur_position_z = 0.0;
    cur_heading_rad = 0.0;
    landing_requested = false;
    local_pose_received = false;
    yaw_locked = false;
    locked_yaw_rad = 0.0;
    takeoff_x = 0.0;
    takeoff_y = 0.0;

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

    // Publish a valid neutral setpoint from startup. Without this initialization
    // MAVROS forwards coordinate_frame=0, which PX4 rejects for SET_POSITION_TARGET_LOCAL_NED.
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
    // 2040 ignores velocity, acceleration/force, and yaw-rate, but keeps position and yaw active.
    // Before the first local pose arrives, keep yaw ignored to avoid commanding yaw=0 at startup.
    cur_target_pose.type_mask = local_pose_received ? 2040 : 3064;
    cur_target_pose.position.x = x;
    cur_target_pose.position.y = y;
    cur_target_pose.position.z = z;
    cur_target_pose.yaw = yaw;
}

void Driver::set_target_pose_current_xy(double z)
{
    if (!local_pose_received) {
        set_target_pose(0.0, 0.0, z, yaw_rad);
        return;
    }

    if (!yaw_locked) {
        locked_yaw_rad = cur_heading_rad;
        yaw_rad = locked_yaw_rad;
        yaw_locked = true;
    }

    set_target_pose(cur_position_x, cur_position_y, z, locked_yaw_rad);
}

void Driver::local_pose_callback(const geometry_msgs::PoseStampedConstPtr& msg)
{
    //ENU坐标系
    //获取无人机当前位置
    cur_position_x = msg->pose.position.x;
    cur_position_y = msg->pose.position.y;
    cur_position_z = msg->pose.position.z;
    cur_heading_rad = tf::getYaw(msg->pose.orientation);

    if (!local_pose_received) {
        local_pose_received = true;
        locked_yaw_rad = cur_heading_rad;
        yaw_rad = locked_yaw_rad;
        yaw_locked = true;
        takeoff_x = cur_position_x;
        takeoff_y = cur_position_y;
        set_target_pose(cur_position_x, cur_position_y, cur_target_pose.position.z, locked_yaw_rad);
    }
}



void Driver::mavros_state_callback(const mavros_msgs::StateConstPtr& msg)
{
    mavros_state = *msg;
}


void Driver::set_target_position_callback(const geometry_msgs::PoseStampedConstPtr& msg)
{
    if (landing_requested) {
        return;
    }

    // 基于世界坐标系直接发送位置信息；默认锁定起飞时 yaw，不跟随外部 yaw_d。
    set_target_pose(msg->pose.position.x, msg->pose.position.y, msg->pose.position.z, locked_yaw_rad);
}
//设置期望角度，只有接收到才有，否则为0
void Driver::set_target_yaw_callback(const std_msgs::Float64ConstPtr& msg)
{
    if (landing_requested) {
        return;
    }

    // 当前实机任务要求 yaw 不动。保留订阅但忽略外部 yaw 命令，避免实验节点或降落流程改变 yaw。
    (void)msg;
    if (!yaw_locked && local_pose_received) {
        locked_yaw_rad = cur_heading_rad;
        yaw_locked = true;
    }
    yaw_rad = locked_yaw_rad;
    set_target_pose(cur_target_pose.position.x, cur_target_pose.position.y, cur_target_pose.position.z, locked_yaw_rad);
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
    landing_requested = true;

    if (local_pose_received) {
        if (!yaw_locked) {
            locked_yaw_rad = cur_heading_rad;
            yaw_locked = true;
        }

        const double land_x = cur_position_x;
        const double land_y = cur_position_y;
        const double land_yaw = locked_yaw_rad;
        const double start_z = cur_position_z;
        const double final_z = 0.15;
        const double descend_rate = 0.20;
        const double dz = start_z - final_z;
        const double duration = std::max(2.0, std::fabs(dz) / descend_rate);
        ros::Rate land_rate(30);
        const ros::Time start_time = ros::Time::now();

        ROS_INFO("OFFBOARD yaw-hold landing: x=%.2f y=%.2f z=%.2f yaw=%.1fdeg",
                 land_x, land_y, start_z, land_yaw * 180.0 / M_PI);

        while (ros::ok()) {
            const double elapsed = (ros::Time::now() - start_time).toSec();
            const double ratio = std::min(1.0, elapsed / duration);
            const double z_sp = start_z + (final_z - start_z) * ratio;

            set_target_pose(land_x, land_y, z_sp, land_yaw);
            local_target_pub.publish(cur_target_pose);
            ros::spinOnce();

            if (ratio >= 1.0 || cur_position_z < 0.20) {
                break;
            }

            land_rate.sleep();
        }
    }

    disarm();
    ROS_INFO("OFFBOARD yaw-hold landing finished, disarm requested");
}

void Driver::offboard()
{
    // cout<<"offboardingng!!!"<<endl;
    landing_requested = false;
    if (local_pose_received && !yaw_locked) {
        locked_yaw_rad = cur_heading_rad;
        yaw_rad = locked_yaw_rad;
        yaw_locked = true;
    }
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
        landing_requested = false;
        if (local_pose_received) {
            takeoff_x = cur_position_x;
            takeoff_y = cur_position_y;
            locked_yaw_rad = cur_heading_rad;
            yaw_rad = locked_yaw_rad;
            yaw_locked = true;
            set_target_pose(takeoff_x, takeoff_y, TAKEOFF_HEIGHT, locked_yaw_rad);
        } else {
            set_target_pose(0, 0, TAKEOFF_HEIGHT, yaw_rad);
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
        local_target_pub.publish(cur_target_pose);//这里一直发布期望位置
        ros::spinOnce();
        loop_rate.sleep();
    }
}
