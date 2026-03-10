#include "uav/Uav_info.h"
#include "uav/desired_start.h"
void Judge_param_load(ros::NodeHandle nh);//通过这个函数判断yaml是否加载到参数服务器中
void Trajectory_plan(Uav_info &iris,ros::NodeHandle &nh);

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
    double hover_z = 1.0;
    nh.param("/wjl/uam/hover_z", hover_z, hover_z);
    iris0.setpoint_pos.z = hover_z;
    Trajectory_plan(iris0,nh);
    ROS_INFO("即将降落");
    iris0.cmd.land();
    ROS_INFO("降落成功！！！！");
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


void Trajectory_plan(Uav_info &iris,ros::NodeHandle &nh){
    ros::Rate r(30);
    //解锁并进入offboard模式并飞到起点上方
    ROS_INFO("即将解锁");
    iris.Set_arm_offboard();
    ROS_INFO("进入offboard模式");
    iris.IsArrived(iris.setpoint_pos.x,iris.setpoint_pos.y,iris.setpoint_pos.z);
    ROS_INFO("开始起飞");
    ros::ServiceClient client = nh.serviceClient<uav::desired_start>("/wjl/start/uav_desired");
    ros::service::waitForService("/wjl/start/uav_desired");
    uav::desired_start start_;
    start_.request.desired_start=10;
    bool flag = client.call(start_);
    // 7.处理响应
    if (flag){
        ROS_INFO("请求正常处理,响应结果:%d",start_.response.desired_sent);
    }else{
        ROS_ERROR("请求处理失败....");
        iris.land_flag=true;
    }
    while(!iris.land_flag && start_.response.desired_sent==3){ 
        r.sleep();
        ros::spinOnce();  
    } 
    ros::Duration(3).sleep();
    iris.IsArrived(iris.setpoint_pos.x,iris.setpoint_pos.y,0.5);
}
