#!/usr/bin/env python

#chmod u+x ~/catkin_ws/src/beginner_tutorials4/src/task_three.py

import math
import rospy
import sys
import ros
from sensor_msgs.msg import LaserScan
from geometry_msgs.msg import Twist

nodeid = str(sys.argv[1])
nodename = 'robot_' + nodeid
selector = int(nodeid)

geo_msg = Twist()
geo_msg.linear.x = 1
geo_msg.angular.z = 0.0

minfrontdist = 0.6
speed = 0.5
edge = False
total = 1

minleft = 1000000.0
minright = 1000000.0
maxleft = -1
maxright = -1

rospy.init_node('movenode', anonymous=True)
pub = rospy.Publisher(nodename + '/cmd_vel', Twist, queue_size=10)
	
def edgefunc(minleft, minright):
	global selector
	
	if selector%2 == 0:
		rotate(-1)
	else:
		rotate(+1)

def go():
	geo_msg.linear.x = 3
	pub.publish(geo_msg)
	rotate(+1)

def rotate(dir):
	geo_msg.linear.x = 0
	geo_msg.angular.z = (dir * math.pi)/2
	pub.publish(geo_msg)

def callback(msg):
	edge = False
	size = len(msg.ranges)
	global total
	global minleft
	global minright
	global maxleft
	global maxright

	for i in range(0, size):

		if float(msg.ranges[i]) < minfrontdist:
			edge = True
	
		if i < size/2:
			minleft = min(minleft, float(msg.ranges[i]))
			maxleft = max(maxleft, float(msg.ranges[i]))
		else:
			minright = min(minright, float(msg.ranges[i]))
			maxright = max(maxright, float(msg.ranges[i]))

		if edge:
			edgefunc(minleft, minright)
			
		else:	
			geo_msg.linear.x = speed
			geo_msg.angular.z = 0.0
			pub.publish(geo_msg)


sub = rospy.Subscriber(nodename + '/base_scan', LaserScan, callback)
rospy.spin()
