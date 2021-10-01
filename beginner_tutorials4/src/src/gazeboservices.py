#!/usr/bin/env python

#chmod u+x ~/catkin_ws/src/beginner_tutorials4/src/gazeboservices.py

import rospy
import sys
from geometry_msgs.msg import Twist
from sensor_msgs.msg import LaserScan
from gazebo_msgs.srv import GetModelState
from gazebo_msgs.srv import SetModelState
from gazebo_msgs.msg import ModelState

rospy.init_node('gazebonode', anonymous=True)
pub = rospy.Publisher('/cmd_vel', Twist, queue_size=10)
flag = True
vel_msg = Twist()

def func():
	global flag
	try:
		flag =False
		state_msg = ModelState()
		state_msg.model_name = 'turtlebot3_burger'
		state_msg.pose.position.x = -1
		state_msg.pose.position.y = -1
		state_msg.pose.position.z = 0
		state_msg.pose.orientation.x = 0
		state_msg.pose.orientation.y = 0	
		state_msg.pose.orientation.z = 0
		state_msg.pose.orientation.w = 0
		rospy.wait_for_service('/gazebo/set_model_state')
		set_state = rospy.ServiceProxy('/gazebo/set_model_state', SetModelState)
		resp = set_state(state_msg)
		
	except rospy.ServiceException, e:
		print "Service call failed: %s" %e
	
	
def callback(msg):
	
	if flag:
		func()
	else:
		if msg.ranges[345] > 0.5 and msg.ranges[0] > 0.5 and msg.ranges[15] > 0.5:
			vel_msg.linear.x = 0.5
			vel_msg.angular.z = 0.0
		else:
			vel_msg.linear.x = 0.0
			vel_msg.angular.z = 0.0

		pub.publish(vel_msg)

sub = rospy.Subscriber('/scan', LaserScan, callback)
rospy.spin()
