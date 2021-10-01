#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Pose2D.h>
#include <tf/tf.h>
#include <math.h>
#include <std_srvs/Empty.h>
#include <beginner_tutorials4/CalculateEuclid.h>

ros::Publisher pub;
geometry_msgs::Twist msg;
geometry_msgs::Pose2D current_pose;

void call_back(const nav_msgs::Odometry::ConstPtr &msg){
	current_pose.x = msg->pose.pose.position.x;		
	current_pose.y = msg->pose.pose.position.y;

  	tf::Quaternion q(
		msg->pose.pose.orientation.x,
		msg->pose.pose.orientation.y,
		msg->pose.pose.orientation.z,
		msg->pose.pose.orientation.w);

  		tf::Matrix3x3 m(q);
 		double roll, pitch, yaw;
		m.getRPY(roll, pitch, yaw);
		current_pose.theta = yaw;
}

double euclidean_distance(float goal_x, float goal_y, ros::NodeHandle n){

  	ros::ServiceClient client = n.serviceClient<beginner_tutorials4::CalculateEuclid>("calc_euc");
  	beginner_tutorials4::CalculateEuclid srv;

  	srv.request.point_one_x = (float) goal_x;
  	srv.request.point_one_y = (float) goal_y;
  	srv.request.point_two_x = (float) current_pose.x;
  	srv.request.point_two_y = (float) current_pose.y;
 	
	ROS_INFO("goal x %lf", (float) goal_x);
	ROS_INFO("goal y %lf", (float) goal_y);
	ROS_INFO("pose x %lf", (float) current_pose.x);
	ROS_INFO("pose y %lf", (float) current_pose.y);

	if(client.call(srv)){
     		ROS_INFO("euc_dist %lf", (float)srv.response.euc_dist);
     	  	return (float)srv.response.euc_dist;
  	} else {
     		ROS_ERROR("Failed to call service calc_euc");
     		return 1;
  	}
}

float linear_vel(float goal_x, float goal_y, float constant, ros::NodeHandle n){
  	return constant * euclidean_distance(goal_x, goal_y, n);	
}

float steering_angle(float goal_x, float goal_y){
  	return atan2((goal_y - (float)current_pose.y), (goal_x - (float)current_pose.x));
}

float angular_vel(float goal_x, float goal_y, float constant, ros::NodeHandle n){
  	return constant * (steering_angle(goal_x, goal_y) - (float)current_pose.theta);
}

//Moves to the given goal
void movetogoal(ros::NodeHandle n, float goal_x, float goal_y, float tolerance){
	pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 1000);
    	ros::Rate loop_rate(10);
	//(euclidean_distance(goal_x, goal_y, n) >= tolerance )
	while(ros::ok() && (current_pose.x <= goal_x ) && (current_pose.y <= goal_y)){

	msg.linear.x = linear_vel(goal_x, goal_y, 0.5, n);
	msg.linear.y = 0.0;
	msg.linear.z = 0.0;

	msg.angular.x = 0.0;
	msg.angular.y = 0.0;
	msg.angular.z = angular_vel(goal_x, goal_y, 0.5, n);

	pub.publish(msg);
	ros::spinOnce();
	loop_rate.sleep();
	}
	
	msg.linear.x = 0;
	msg.angular.z = 0;
	pub.publish(msg);
}

int main(int argc, char **argv) {
	ros::init(argc, argv, "gotogoal");
	ros::NodeHandle n;

	ros::Subscriber sub = n.subscribe("/odom", 1000, call_back);
 	
	float goal_x, goal_y;
/*
	//1st way is getting parameters from paramsproject.yaml
	n.getParam("goal_x",goal_x);
	n.getParam("goal_y",goal_y);

*/	
	//2nd way is geting parameters from finallaunch.launch
	n.getParam("goal_xl",goal_x);
	n.getParam("goal_yl",goal_y);
	
	movetogoal(n, goal_x, goal_y, 0.5);
	
	ros::spin();

	return 0;
}

