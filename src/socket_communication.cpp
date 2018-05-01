#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "iostream"
#include "sstream"
#include "string"
#include "math.h"

#define PORT 7200

geometry_msgs::Point coordinates;

void coordinatesCallback(geometry_msgs::Point recievedCoordinates){
  coordinates = recievedCoordinates;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "socket_communication");
  ros::NodeHandle nh;
  ros::Subscriber coordinates_sub = nh.subscribe("coordinates", 1, coordinatesCallback);
  ros::Rate loop_rate(50);

  char coord_x[11] = {0};
  char coord_y[11] = {0};
  char coord_z[11] = {0};

  char coordinates_char[39] = {0};

  int send_return;
  char confirm[1] = {0};

  struct sockaddr_in si_me;
  int sock, new_socket;

  //create socket
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    ROS_ERROR("Can't create socket!");
  }
  else{
    ROS_INFO("Socket created!");
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
  else{
    ROS_INFO("Socket binded!");
  }

  //listen for connections
  if((listen(sock, 1)) == -1){
    ROS_ERROR("Listen failed!");
  }
  else{
    ROS_INFO("Socket is listening!");
  }

  //accept connections
  if((new_socket = accept(sock, (struct sockaddr *)&si_me, (socklen_t*)&si_me)) == -1){
    ROS_ERROR("Accept failed!");
  }
  else{
    ROS_INFO("Connection accepted!");
  }

  int i = 0;

  while(nh.ok()) {

    //ROS_INFO("Creating sub strings!");
    snprintf(coord_x, sizeof(coord_x), "%11.4f", coordinates.x);
    snprintf(coord_y, sizeof(coord_y), "%11.4f", coordinates.y);
    snprintf(coord_z, sizeof(coord_z), "%11.4f", coordinates.z);
    /*
    float x = 300;
    float y = -20 - i;
    float z = 500;

    snprintf(coord_x, sizeof(coord_x), "%11.4f", x);
    snprintf(coord_y, sizeof(coord_y), "%11.4f", y);
    snprintf(coord_z, sizeof(coord_z), "%11.4f", z);
    */

    //ROS_INFO("Creating message string!");
    snprintf(coordinates_char, sizeof(coordinates_char), "%sx%sy%sz", coord_x, coord_y, coord_z);

    //ROS_INFO("Checking message");
    if(!std::isnan(coordinates.x)  && !std::isnan(coordinates.y) && !std::isnan(coordinates.z)){
      ROS_INFO("Message is OK! Iteracia %i", i);
      if((send(new_socket, &coordinates_char[0], sizeof(coordinates_char), MSG_DONTWAIT)) == -1){
        ROS_ERROR("Can't send message!");
      }
      else{
        ROS_INFO("%s", coordinates_char);
        ROS_INFO("%f", coordinates.x);
        ROS_INFO("%f", coordinates.y);
        ROS_INFO("%f", coordinates.z);
        ROS_INFO("\n");

        recv(new_socket, &confirm, 1, 0);
        ROS_INFO("%s", confirm);
        //ROS_INFO("Number of bytes sent: %i", sizeof(coordinates_char));
      }
    }

    ROS_INFO("Spinning!");
    ros::spinOnce();
    i++;
    loop_rate.sleep();
  }

  close(sock);
  return 0;
}
