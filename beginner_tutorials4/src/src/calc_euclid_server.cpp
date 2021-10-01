#include "ros/ros.h"
#include "math.h"
#include "beginner_tutorials4/CalculateEuclid.h"

bool euc_dist(beginner_tutorials4::CalculateEuclid::Request &req, beginner_tutorials4::CalculateEuclid::Response &res) {

  res.euc_dist = sqrt(pow((req.point_one_x - req.point_two_x),2) + pow((req.point_one_y - req.point_two_y),2));

  ROS_INFO("request: x1=%f, y1=%f x2=%f, y2=%f", (float)req.point_one_x, (float)req.point_one_y, (float)req.point_two_x, (float)req.point_two_y);

  ROS_INFO("response: euc_dist=%lf", (float)res.euc_dist);
  return true;
}

int main(int argc, char **argv) {

 ros::init(argc, argv, "calc_euc_server");
 ros::NodeHandle n;

 ros::ServiceServer service = n.advertiseService("calc_euc",euc_dist);
 
 ROS_INFO("Ready");
 ros::spin();

 return 0;
}
