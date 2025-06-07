cmake_minimum_required(VERSION 3.0.2)
project(uav)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  message_generation
  std_msgs
  dynamic_reconfigure
)



################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a exec_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a exec_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
add_message_files(
  FILES
  xyz_yaw_d.msg
)

## Generate services in the 'srv' folder
add_service_files(
  FILES
  desired_start.srv
)

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
  std_msgs
)

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## Generate dynamic reconfigure parameters in the 'cfg' folder
generate_dynamic_reconfigure_options(
  cfg/uav_cmd.cfg
  # cfg/DynReconf2.cfg
)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES uav
 CATKIN_DEPENDS roscpp rospy std_msgs message_runtime
#  DEPENDS system_lib

)

###########
## Build ##
###########

# 定义与飞行器有关的源文件变量
set(SRC_UAV src/uav/Uav_info.cpp src/uav/Commander.cpp src/other/Print_.cpp)
# 位置估计节点
add_executable(pos_estimator0 src/estimator/z_pos_estimator0.cpp src/estimator/pos_estimator.cpp src/other/Print_.cpp)
target_link_libraries(pos_estimator0 ${catkin_LIBRARIES})
add_executable(gazebo_pose0 src/estimator/z_gazebo_pose0.cpp src/other/Print_.cpp)
target_link_libraries(gazebo_pose0 ${catkin_LIBRARIES})
# 飞行控制节点
add_executable(Driver0 src/uav/z_Driver0.cpp src/uav/Driver.cpp src/other/Print_.cpp)
target_link_libraries(Driver0 ${catkin_LIBRARIES})
add_executable(uav_guidefly_control src/uav_guidefly_control.cpp ${SRC_UAV})
target_link_libraries(uav_guidefly_control ${catkin_LIBRARIES})

add_executable(desired_uav_fly src/desired_uav_fly.cpp ${SRC_UAV})
target_link_libraries(desired_uav_fly ${catkin_LIBRARIES})

# 动态参数节点
add_executable(dynamic_guidefly src/dynamic_guidefly.cpp)
target_link_libraries(dynamic_guidefly ${catkin_LIBRARIES})




## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
# include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  "/usr/include/eigen3"
)


#############
## Install ##
#############



#############
## Testing ##
#############

