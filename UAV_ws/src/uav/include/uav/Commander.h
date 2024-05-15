#include "common.h"
//move设置发布的位置信息
//说明：Commander主要功能是设置当前模式名（activity.data），并发布到Driver中接受设置
class Commander
{
public:
  geometry_msgs::PoseStamped pose;//位置

  Commander(ros::NodeHandle& nh,std::string model_name);

  ~Commander();

  //这里的模式就是将activity.data赋值并发布，在Driver.cpp接受设置模式
  void offboard();//将activity.data = "OFFBOARD"并发布，然后在Driver.cpp中设置
  void arm();
  void disarm();
  void move(double x, double y, double z, string frame = "ENU");// 给位置信息，frame是坐标系，只有FLU和ENU，FLU是机体坐标系，ENU是世界坐标系
  void turn(double yaw);//发布期望偏航角,发送的角度
  void hover();//悬停
  void land();//着陆
  void takeoff();
  void set_pose(double x, double y, double z, string frame = "ENU");//设置坐标

private:
  ros::Publisher position_target_pub;//发布位姿目标
  ros::Publisher yaw_target_pub;//
  ros::Publisher custom_activity_pub;//发布

  std_msgs::String activity;//当前模式
  std_msgs::Float64 yaw;//角度
};
