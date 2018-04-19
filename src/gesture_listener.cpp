#include "ros/ros.h"
#include "ros/console.h"
#include "std_msgs/Float64.h"
#include "tf/transform_listener.h"
#include "geometry_msgs/Twist.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "gesture_listener");
  ros::NodeHandle nh;

  ros::Publisher coordinates_pub = nh.advertise<geometry_msgs::Point>("coordinates", 1);

  tf::TransformListener tfListener;

  ros::Rate loop_rate(50);

  while(nh.ok()){

    //transform hand torso
    tf::StampedTransform transform_right_hand;
    tf::StampedTransform transform_torso;
    try{
      tfListener.lookupTransform("/right_hand_1", "/openni_depth_frame", ros::Time(0), transform_right_hand);
      tfListener.lookupTransform("/torso_1", "/openni_depth_frame", ros::Time(0), transform_torso);
    }
    catch(tf::TransformException ex){
      ROS_ERROR("%s", ex.what());
      ros::Duration(1.0).sleep();
    }

    //get xyz coordinates
    geometry_msgs::Point coordinates_right_hand;

    coordinates_right_hand.x = (transform_right_hand.getOrigin().x() - transform_torso.getOrigin().x()) * 1000;
    coordinates_right_hand.y = (transform_right_hand.getOrigin().y() - transform_torso.getOrigin().y()) * 1000;
    coordinates_right_hand.z = (transform_right_hand.getOrigin().z() - transform_torso.getOrigin().z()) * 1000;

    //publish coordinates
    coordinates_pub.publish(coordinates_right_hand);

    loop_rate.sleep();
  }

  return 0;
}
