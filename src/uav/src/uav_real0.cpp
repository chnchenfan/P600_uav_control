#include "uav/Uav_info.h"
//通过这个函数判断yaml是否加载到参数服务器中
void Judge_param_load(ros::NodeHandle nh);
//主要是解锁并进入offboard模式，接着就是IsArrived判断是否到达指定位置，iris.Set_pose(x,y,z);设置期望位置
//判断是否到达指定位置
//实物测试，起飞后，然后慢慢降落
void path_planning(Uav_info &iris);


int main(int argc, char *argv[])
{   
    setlocale(LC_ALL,"");
    ros::init(argc, argv, "uav_real0");
    ros::NodeHandle nh;
    Judge_param_load(nh);//判断是否加载yaml参数
    std::string model_name;
    nh.getParam("/model_name0", model_name);    
    Uav_info iris0(nh,model_name);
    iris0.setpoint_pos.z=0.5;
    Print_green("uav_real0节点启动!!!");
    path_planning(iris0);//实物测试
    ROS_INFO("即将降落");
    iris0.cmd.land();
    ROS_INFO("降落成功！！！！");
    //让Driver节点一直运行
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
    std::string model_name;
    nh.getParam("/model_name0", model_name);    
    ROS_INFO("uav节点yaml文件加载成功！");
    if(flag==0 && gn=="g0"){
        Print_green("未使用组名!!!");
    }else if(flag==1 && gn=="g1"){
        Print_green("使用了组名,组名为!!!"+model_name);
    }else{
        std::cout<<"/group_flag:"<<flag<<" /ns_flag"<<gn<<std::endl;
        ROS_ERROR("请看uav_yaml文件的group_flag和px4启动launch文件的ns_flag标志是否匹配");
        ROS_ERROR("px4启动launch文件查看gazebo_px4_startup.launch或者real_px4_startup.launch");
        ROS_ERROR("group_flag:0 对应 ns_flag:g0;   group_flag:1 对应 ns_flag:g1");
        exit(0);
    }
}

void path_planning(Uav_info &iris){
    ros::Rate r(30);
    while(ros::ok()){      
        if(iris.RC_control==1)
        {    
            //解锁并进入offboard模式并飞到起点上方
            ROS_INFO("即将解锁");
            iris.Set_arm_offboard();
            ROS_INFO("offboard 切换成功！！！！");
            ROS_INFO("炸鸡正在去起点上方");
            iris.IsArrived(iris.setpoint_pos.x,iris.setpoint_pos.y,iris.setpoint_pos.z);
            //下面就写自己的逻辑了
            // 单纯起飞并降落
            iris.IsArrived(0,0,0.8);
            // iris.IsArrived(0,0,0.6);
            
            // // 正方形飞行
            // iris.IsArrived(0,0,0.5);
            // iris.IsArrived(1,0,0.5);
            // iris.IsArrived(1,1,0.5);
            // iris.IsArrived(0,1,0.5);
            // iris.IsArrived(0,0,0.5);
            // iris.IsArrived(0,0,0.4);     
            break;
        }else{
            std::cout<<"遥控器控制模式为：手动模式！"<<std::endl;
        }
        r.sleep();
        ros::spinOnce();   
    }
}