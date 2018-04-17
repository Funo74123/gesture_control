cmake_minimum_required(VERSION 2.8.3)

project(gesture_control)

find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs geometry_msgs tf)

catkin_package()

include_directories(${catkin_INCLUDE_DIRS})

add_executable(gesture_listener src/gesture_listener.cpp)
target_link_libraries(gesture_listener ${catkin_LIBRARIES})
