#include "estimator/pos_estimator.h"
int main(int argc,char** argv)
{
    setlocale(LC_ALL, "");
    ros::init(argc, argv, "driver_0");
    ros::NodeHandle nh;
    std::string modle_name;
    nh.getParam("modle_name", modle_name);;  
    Pos_estimator pos_estimator(nh,modle_name);
    pos_estimator.run();
    return 0;
}