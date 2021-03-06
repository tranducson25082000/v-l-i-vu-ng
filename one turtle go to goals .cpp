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
const double tolerance =1e-3;//pow(10,-8);
bool firstTime=true;
float rate = 100;//600;

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
    firstTime=false;
    
   
}

int main(int argc, char** argv)
{
     ros::init(argc, argv, "myturtle_control");
    
     ros::NodeHandle h;
     pub = h.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);
     ros::Subscriber sub =h.subscribe("/turtle1/pose", 1000, poseCallback);
     ros::Rate loopRate(rate);
     cout<<"argc=:"<<argc;
     double t0= ros::Time::now().toSec();
     double a;
     for(int i=1;i<argc;i+=2)
     {   
         double target_x0=atof(argv[i]);
         double target_y0=atof(argv[i+1]);
     
         cout<<"argv:"<<argv;
         while (ros::ok())
          {
             loopRate.sleep();
             ros::spinOnce();
             if(!firstTime)
             {
             cout <<fixed<<setprecision(2)<< current_pose.x << " " << current_pose.y << " " << current_pose.theta << endl;
             double distance = sqrt( pow(target_x0-current_pose.x, 2) + pow(target_y0-current_pose.y, 2) );
           
             if (distance < tolerance) { 
                pub.publish(getMessage(0,0));
                double t1= ros::Time::now().toSec();
                cout <<"Thời gian đi hết qua tất cả các điểm là : " << t1 -t0 << endl;
                break;
             }
             
             double dx = target_x0 - current_pose.x, dy = target_y0 - current_pose.y, theta = current_pose.theta;
          
             double angle  = acos ((cos(theta)*dx+sin(theta)*dy) / distance);//tinh xem goc o vi tri nao
             
             if (angle < PI/2) {  a = 1;}
             else a=-1;
             double dalpha = asin((cos(theta) * dy- sin(theta) * dx) / distance);
            
             pub.publish( getMessage( a*min(18*distance, 20.0), a*20*dalpha ));//10,10.0,35;
            
             }
          }
          }
     
return 0;
}

