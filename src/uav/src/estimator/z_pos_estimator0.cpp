#include "estimator/pos_estimator.h"
int main(int argc,char** argv)
{
    setlocale(LC_ALL, "");
    ros::init(argc, argv, "pos_estimator0");
    ros::NodeHandle nh;
    std::string model_name;
    nh.getParam("/model_name0", model_name);;  
    Pos_estimator pos_estimator(nh,model_name);
    Printf_green("pos_estimator0节点启动!!!");
    pos_estimator.run();
    return 0;
}