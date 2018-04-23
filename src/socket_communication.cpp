#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "iostream"
#include "sstream"

#define PORT 7200

geometry_msgs::Point coordinates;

void coordinatesCallback(geometry_msgs::Point recievedCoordinates){
  coordinates = recievedCoordinates;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "socket_communication");
  ros::NodeHandle nh;
  ros::Subscriber coordinates_sub = nh.subscribe("coordinates", 1000, coordinatesCallback);

  struct sockaddr_in si_me;
  int sock, new_socket;
  int slength = sizeof(si_me);
  std::string coordinates_string;
  std::ostringstream coordinates_convert;

  //create socket
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    ROS_ERROR("Can't create socket!");
  }

  //zero out the structure
  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind socket to port
  if((bind(sock, (struct sockaddr*)&si_me, sizeof(si_me))) == -1){
    ROS_ERROR("Can't bind the socket to the port!");
  }

  //listen for connections
  if((listen(sock, 1)) == -1){
    ROS_ERROR("Listen failed!");
  }

  //accept connections
  if((new_socket = accept(sock, (struct sockaddr *)&si_me, (socklen_t*)&slength)) == -1){
    ROS_ERROR("Accept failed!");
  }

  while(nh.ok()) {

    /*
    ROS_INFO("x = %f", coordinates.x);
    ROS_INFO("y = %f", coordinates.y);
    ROS_INFO("z = %f", coordinates.z);
    */


    float coord_x = 111.111111111111111;//coordinates.x;
    float coord_y = 222.222222222222222;//coordinates.y;
    float coord_z = 333.333333333333333;//coordinates.z;


    /*float coord_x = coordinates.x;
    float coord_y = coordinates.y;
    float coord_z = coordinates.z;
*/
    coordinates_convert  << coord_x << "x" << coord_y << "y" << coord_z << "z";
    coordinates_string = coordinates_convert.str();

    //ROS_INFO_STREAM(coordinates_string);

    if((send(sock, &coordinates_string, sizeof(coordinates_string), 0)) == -1){//, (struct sockaddr*) &si_other, slength) == -1){
     // ROS_ERROR("Can't send message!");
    }

    ros::spinOnce();
  }

  close(sock);
  return 0;
}
