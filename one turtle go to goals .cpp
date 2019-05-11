
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>
#include<cmath>
#include <iostream>
#include <algorithm>
#include<iomanip>
using namespace std;

ros::Publisher pub;
const float PI = 3.14159265;
float rate = 200;
turtlesim::Pose current_pose;

geometry_msgs::Twist getMessage(double linear_x, double angular_z)
{
    geometry_msgs::Twist msg;
    msg.linear.x = linear_x;
    msg.angular.z = angular_z;
    return msg;
}

void poseCallback(const turtlesim::Pose::ConstPtr& msg)
{
    current_pose = *msg;
}

int main(int argc, char** argv)
{
     ros::init(argc, argv, "myturtle_control");
    
     ros::NodeHandle h;
     pub = h.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);
     ros::Subscriber sub =h.subscribe("/turtle1/pose", 1000, poseCallback);
     ros::Rate loopRate(rate);
     cout<<"argc=:"<<argc;

     for(int i=1;i<argc;i+=2)
     {
         double x0=atof(argv[i]);
         double y0=atof(argv[i+1]);
         const double tolerance =pow(10,-8);
         cout<<"argv:"<<argv;
        //  tinh goc luc di den diem cua rua
         double dx0 = x0 - current_pose.x, dy0 = y0 - current_pose.y, theta0 = current_pose.theta;
         double distance0 = sqrt( pow(x0-current_pose.x, 2) + pow(y0-current_pose.y, 2) );
         double di = acos((cos(theta0)*dx0+sin(theta0)*dy0)/distance0);
         double a;

         if (di < PI/2) {
                 a = 1;
             }
         else {
                 a = -1;
            }
         while (ros::ok())
          {
             loopRate.sleep();
             ros::spinOnce();
             cout <<fixed<<setprecision(2)<< current_pose.x << " " << current_pose.y << " " << current_pose.theta << endl;
             double distance = sqrt( pow(x0-current_pose.x, 2) + pow(y0-current_pose.y, 2) );
             if (distance < tolerance) { 
                pub.publish(getMessage(0,0));
                break;
             }
             double dx = x0 - current_pose.x, dy = y0 - current_pose.y, theta = current_pose.theta;
             double dalpha = asin ((cos(theta)*dy-sin(theta)*dx) / distance);
             pub.publish( getMessage( a*min(20*distance, 25.0), a*30*dalpha ));
          }
     }
return 0;
}
