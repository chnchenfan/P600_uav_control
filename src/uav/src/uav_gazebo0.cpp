#include "uav/Uav_info.h"
void Judge_param_load(ros::NodeHandle nh);//通过这个函数判断yaml是否加载到参数服务器中
void Gazebo_test(Uav_info &iris);

int main(int argc, char *argv[])
{   
    setlocale(LC_ALL,"");
    ros::init(argc, argv, "uav_gazebo0");
    ros::NodeHandle nh;
    Judge_param_load(nh);//判断是否加载yaml参数
    Printf_green("uav_gazebo0节点启动!!!");
    std::string model_name;
    nh.getParam("/model_name0", model_name);
    Uav_info iris0(nh,model_name);
    iris0.setpoint_pos.z=0.5;
    Gazebo_test(iris0);//仿真测试
    ROS_INFO("即将降落");
    iris0.cmd.land();
    ROS_INFO("降落成功！！！！");
    ros::spin();
}

void Judge_param_load(ros::NodeHandle nh){
    while (!nh.hasParam("/load_param_flag") && !nh.hasParam("/ns_flag"))
  	{
    	ROS_INFO("uav节点等待参数加载...");
    	ros::Duration(1.0).sleep();
  	}
    int flag;
    std::string gn;
    nh.getParam("/ns_flag", gn);
    nh.getParam("/group_flag", flag);
    ROS_INFO("uav节点yaml文件加载成功！");
    if(flag==0 && gn=="g0"){
        Printf_green("未使用组名!!!");
    }else if(flag==1 && gn=="g1"){
        Printf_green("使用了组名,组名为!!!");
    }else{
        std::cout<<"/group_flag:"<<flag<<" /ns_flag:"<<gn<<std::endl;
        ROS_ERROR("请看uav_yaml文件的group_flag和px4启动launch文件的ns_flag标志是否匹配");
        ROS_ERROR("px4启动launch文件查看gazebo_px4_startup.launch或者real_px4_startup.launch");
        ROS_ERROR("group_flag:0 对应 ns_flag:g0;   group_flag:1 对应 ns_flag:g1");
        exit(0);
    }
}

//仿真测试
void Gazebo_test(Uav_info &iris){
    ros::Rate r(30);
    while(ros::ok()){ 
        //解锁并进入offboard模式并飞到起点上方
        ROS_INFO("即将解锁");
        iris.Set_arm_offboard();
        ROS_INFO("offboard 切换成功！！！！");
        ROS_INFO("炸鸡正在去起点上方");
        iris.IsArrived(iris.setpoint_pos.x,iris.setpoint_pos.y,iris.setpoint_pos.z);
        //正方形飞行
        iris.IsArrived(0,0,1);
        iris.IsArrived(1,0,1);
        iris.IsArrived(1,1,1);
        iris.IsArrived(0,1,1);
        iris.IsArrived(0,0,1);
        iris.IsArrived(0,0,0.5);        
        break;
        r.sleep();
        ros::spinOnce();  
    } 
}