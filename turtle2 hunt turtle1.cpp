
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>

#include <iostream>
#include <algorithm>
using namespace std;

ros::Publisher pub;
const float PI = 3.14159265;
float tx,ty;
int state=0;
 const double tolerance = 1e-2;
float rate = 5;
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

void poseCallback2(const turtlesim::Pose::ConstPtr &msg)
{

    tx = msg->x, ty = msg->y;
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "myturtle_control");
    ros::NodeHandle h;
    pub = h.advertise<geometry_msgs::Twist>("turtle2/cmd_vel", 1000);
    ros::Subscriber sub2 =
        h.subscribe("/turtle1/pose", 1000, poseCallback2);
      ros::Subscriber sub =
        h.subscribe("/turtle2/pose", 1000, poseCallback);
           
        
    ros::Rate loopRate(rate);

   

    while (ros::ok())
    {


       
  
        
       
    
       while (ros::ok()) {
            loopRate.sleep();
            ros::spinOnce();
           cout << current_pose.x << " " << current_pose.y << " " << current_pose.theta << endl;

            // tính khoảng cách đến đích
           double distance = sqrt( pow(tx-current_pose.x, 2) + pow(ty-current_pose.y, 2) );
            if (distance < tolerance) { // dừng robot khi đến đủ gần
                pub.publish(getMessage(0,0));
                break;
           }
            // double alpha = atan2( y0-current_pose.y, x0-current_pose.x ),a_z;
            // tính các vector đến đích và hướng hiện thời
            double dx = tx - current_pose.x, dy = ty - current_pose.y, theta = current_pose.theta;
            // sử dụng tích có hướng của vector để tính sin của góc lệch (sau đó dùng arcsin)
           double dalpha = asin ((cos(theta)*dy-sin(theta)*dx) / distance);
            
            // tạo message bằng phương pháp điều khiển PID (có giới hạn vận tốc)
            // Ở đây dùng hệ số K_p = 1.0, K_i = K_d = 0.0
            geometry_msgs::Twist msg = getMessage(
               min(1*distance, 10.0),
                4*dalpha
           );

            pub.publish(msg);
       /// }
    }
    return 0;
}

