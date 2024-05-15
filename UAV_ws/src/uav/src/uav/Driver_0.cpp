#include "Driver.h"


int main(int argc,char** argv)
{
  setlocale(LC_ALL, "");
  ros::init(argc, argv, "driver_0");
  ros::NodeHandle nh;
  Driver driver(nh,"iris_0");
  driver.start();
  return 0;
}