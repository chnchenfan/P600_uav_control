#include "../../include/px42t265_bridge/px42t265_bridge.h"

using namespace bridge;

int main(int argc, char** argv) {
  ros::init(argc, argv, "Px42t265_bridge_node");
  ros::NodeHandle nh("~");
  px42t265_bridge Bridge(nh);

  ros::spin();

  Bridge.worker_.join();

  return 0;
}
