#include <geometry_msgs/Twist.h>
#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
// Here we are including all the headers necessary to use the most common public
// pieces of the ROS system. Always we create a new C++ file, we will need to
// add this include.

float linX, angZ; // Starting global variables linear x and Angular z

void counterCallback(const sensor_msgs::LaserScan::ConstPtr &msg) {
  ROS_INFO("%f", msg->ranges[360]); // We print the distance to an obstacle in
                                    // front of the robot; calling the counter
                                    // and LaserScan

  // If the distance to an obstacle in front of the robot is bigger than 1
  // meter, the robot will move forward linearly
  if (msg->ranges[360] > 1) {
    linX = 0.25;
    angZ = 0.0;
  }

  // If the robot has made it out the maze, it will stop
  if (msg->ranges[719] > 2 && msg->ranges[0] > 2) {
    linX = 0;
    angZ = 0.0;
  }
  // If the distance to an obstacle in front of the robot is smaller than 1
  // meter, the robot will turn left
  if (msg->ranges[360] < 1) {
    if (msg->ranges[719] < 1) {
      linX = 0.0;
      angZ = -1.42;
    } else if (msg->ranges[0] < 1) {
      linX = 0.0;
      angZ = 1.6;
    } else {
      linX = 0;
      angZ = 1.42;
    }
  }
}

int main(int argc, char **argv) { // We start the main C++ program

  ros::init(
      argc, argv,
      "turtlebot_maze_node"); // We initiate a ROS node called topic_publisher
  ros::NodeHandle nh; // We create a handler for the node. This handler will
                      // actually do the initialization of the node.

  ros::Subscriber sub =
      nh.subscribe("/kobuki/laser/scan", 1000,
                   counterCallback); // Create a Subscriber object that will
                                     // call the 'callback' function each time
                                     // message is received on Topic 'counter'
  ros::Publisher pub = nh.advertise<geometry_msgs::Twist>(
      "cmd_vel", 1000); // Create a publisher object
  geometry_msgs::Twist move;

  ros::Rate rate(1); // Create a rate object of 1Hz frequency

  while (ros::ok()) {

    move.linear.x = linX;
    move.angular.z = angZ;
    pub.publish(move); // Publishes the cmd_vel based what is being updated in
                       // the callback function

    rate.sleep();
    ros::spinOnce();
  }

  return 0;
}
