#include "ros/ros.h"
#include "ros/console.h"
#include "std_msgs/Float64.h"
#include "tf/transform_listener.h"
#include "geometry_msgs/Twist.h"
#include "iostream"
#include "sstream"
#include "fstream"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "gesture_listener");
  ros::NodeHandle nh;

  ros::Publisher coordinates_pub = nh.advertise<geometry_msgs::Point>("coordinates", 1, true);

  tf::TransformListener tfListener;

  std::ofstream x_stream;
  std::ofstream y_stream;
  std::ofstream z_stream;

  ros::Rate loop_rate(50);

  char coord_x[11] = {0};
  char coord_y[11] = {0};
  char coord_z[11] = {0};

  try{
    std::cout << "robim fily" << std::endl;
    x_stream.open("/home/michal/Documents/DP_text/Xraw.txt");
    y_stream.open("/home/michal/Documents/DP_text/Yraw.txt");
    z_stream.open("/home/michal/Documents/DP_text/Zraw.txt");
  }
  catch(std::exception e){
    ROS_ERROR("Can't open file!");
  }

  while(nh.ok()){

    //transform hand torso
    tf::StampedTransform transform_right_hand;
    tf::StampedTransform transform_torso;
    try{
      tfListener.lookupTransform("/left_hand_1", "/openni_depth_frame", ros::Time(0), transform_right_hand);
      tfListener.lookupTransform("/torso_1", "/openni_depth_frame", ros::Time(0), transform_torso);
    }
    catch(tf::TransformException ex){
      ROS_ERROR("%s", ex.what());
      ros::Duration(1.0).sleep();
    }

    //get xyz coordinates
    geometry_msgs::Point coordinates_right_hand;

    coordinates_right_hand.y = (transform_right_hand.getOrigin().x() - transform_torso.getOrigin().x()) * 1000;
    coordinates_right_hand.z = (transform_right_hand.getOrigin().y() - transform_torso.getOrigin().y()) * 1000;
    coordinates_right_hand.x = (transform_right_hand.getOrigin().z() - transform_torso.getOrigin().z()) * 1000;

    ROS_INFO("x = %f", coordinates_right_hand.x);
    ROS_INFO("y = %f", coordinates_right_hand.y);
    ROS_INFO("z = %f", coordinates_right_hand.z);
    ROS_INFO("\n");

    snprintf(coord_x, sizeof(coord_x), "%11.4f", coordinates_right_hand.x);
    snprintf(coord_y, sizeof(coord_y), "%11.4f", coordinates_right_hand.y);
    snprintf(coord_z, sizeof(coord_z), "%11.4f", coordinates_right_hand.z);

    x_stream << coord_x << std::endl;
    y_stream << coord_y << std::endl;
    z_stream << coord_z << std::endl;

    //publish coordinates
    coordinates_pub.publish(coordinates_right_hand);

    loop_rate.sleep();
  }

  return 0;
}
