#include "uav/Uav_info.h"
void Judge_param_load(ros::NodeHandle nh);//通过这个函数判断yaml是否加载到参数服务器中
void Gazebo_test(Uav_info &iris);

int main(int argc, char *argv[])
{   
    setlocale(LC_ALL,"");
    ros::init(argc, argv, "uav_gazebo0");
    ros::NodeHandle nh;
    Judge_param_load(nh);//判断是否加载yaml参数
    Print_green("uav_gazebo0节点启动!!!");
    std::string model_name;
    nh.getParam("/model_name0", model_name);
    Uav_info iris0(nh,model_name);
    iris0.setpoint_pos.z=0.5;
    Gazebo_test(iris0);//仿真测试

    ROS_INFO("收到 land_flag，开始请求 AUTO.LAND");
    ros::Rate landing_rate(10);
    int land_request_count = 0;
    while (ros::ok() && !iris0.IsAutoLandMode()) {
        iris0.cmd.land();
        ++land_request_count;
        if (land_request_count % 10 == 0) {
            ROS_WARN("等待进入 AUTO.LAND，当前模式=%s，已重发 LAND %d 次",
                     iris0.current_mavros_state.mode.c_str(), land_request_count);
        }
        ros::spinOnce();
        landing_rate.sleep();
    }

    if (!ros::ok()) {
        return 0;
    }

    ROS_INFO("已进入 AUTO.LAND，等待无人机接地");
    while (ros::ok() && !iris0.IsOnGround()) {
        if (!iris0.IsAutoLandMode()) {
            ROS_WARN_THROTTLE(1.0, "降落过程中退出 AUTO.LAND，重新请求 LAND");
            iris0.cmd.land();
        }
        ros::spinOnce();
        landing_rate.sleep();
    }

    if (ros::ok()) {
        ROS_INFO("检测到已接地，发送 DISARM");
        iris0.cmd.disarm();
        ros::Duration(1.0).sleep();
    }
    ROS_INFO("降落流程结束");
    ros::spin();
}

void Judge_param_load(ros::NodeHandle nh){
    while (!nh.hasParam("/load_param_flag") && !nh.hasParam("/ns_flag"))
  	{
        std::cout<<"uav节点等待参数加载..."<<std::endl;
    	// ROS_INFO("uav节点等待参数加载...");
    	ros::Duration(1.0).sleep();
  	}
    int flag;
    std::string gn;
    nh.getParam("/ns_flag", gn);
    nh.getParam("/group_flag", flag);
    std::cout<<"uav节点yaml文件加载成功！"<<std::endl;
    if(flag==0 && gn=="g0"){
        Print_green("未使用组名!!!");
    }else if(flag==1 && gn=="g1"){
        Print_green("使用了组名!!!");
    }else{
        std::cout<<"/group_flag:"<<flag<<" /ns_flag:"<<gn<<std::endl;
        Print_red("请看uav_yaml文件的group_flag和px4启动launch文件的ns_flag标志是否匹配");
        Print_red("px4启动launch文件查看gazebo_px4_startup.launch或者real_px4_startup.launch");
        Print_red("group_flag:0 对应 ns_flag:g0;   group_flag:1 对应 ns_flag:g1");
        exit(0);
    }
}


void Gazebo_test(Uav_info &iris){
    ros::Rate r(30);
    //解锁并进入offboard模式并飞到起点上方
    ROS_INFO("即将解锁");
    iris.Set_arm_offboard();
    ROS_INFO("进入offboard模式");
    iris.IsArrived(iris.setpoint_pos.x,iris.setpoint_pos.y,iris.setpoint_pos.z);
    ROS_INFO("开始起飞");
    while(!iris.land_flag){ 
        // if(iris.guidefly_flag){
        //     iris.IsArrived(iris.setpoint_pos.x,iris.setpoint_pos.y,iris.setpoint_pos.z);
        //     iris.guidefly_flag=false;
        // }
        r.sleep();
        ros::spinOnce();  
    } 
}