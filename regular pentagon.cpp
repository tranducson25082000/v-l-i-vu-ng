
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Pose.h>

#include <iostream>
#include <queue>
#include <iomanip>
using namespace std;

float x, y, theta, v, vt;
float prevx, prevy;
int state = 0;
float target_angle;
float target_distance;
ros::Publisher pub;
const float PI = 3.14159265;
float rate = 30;

geometry_msgs::Twist getMessage(double linear_x, double angular_z)
{
    geometry_msgs::Twist msg;
    msg.linear.x = linear_x;
    msg.angular.z = angular_z;
    return msg;
}

void handleStateRotate()
{
    if (abs(target_angle-theta) > 1.0/rate) {
        pub.publish(getMessage(0, (target_angle-theta)));
    } else if (abs(target_angle-theta) > 1e-3) {
        pub.publish(getMessage(0, (target_angle-theta)*rate));
    } else {
        pub.publish(getMessage(0, 0));
        state = 0;
    }
}

void handleStateStraightForward()
{
    cout << "target=" << target_distance << endl;
    if (target_distance <= 1e-6) {
        pub.publish(getMessage(0, 0));
        state = 0;
    } else {
        if (target_distance > 1.0 / rate)
            pub.publish(getMessage(1, 0));
        else
            pub.publish(getMessage(target_distance*rate, 0));
    }
}

void poseCallback(const turtlesim::Pose::ConstPtr& msg)
{
    static bool firstCall = true;
    // cout << "firstCall=" << boolalpha << firstCall << endl;
    prevx = x, prevy = y;
    // cout << "x=" << msg->x << " y=" << msg->y 
    //      << " theta=" << msg->theta << " v=" << msg->linear_velocity
    //      << " vtheta=" << msg->angular_velocity << endl;
    x = msg->x, y = msg->y, theta = msg->theta, 
    v = msg->linear_velocity, vt = msg->angular_velocity;

    float dist = sqrt((x-prevx)*(x-prevx)+(y-prevy)*(y-prevy));
    if (!firstCall) target_distance -= dist;
    firstCall = false;
}

void rotate(float angle)
{
    state = 1;
    target_angle = angle;
}

void straight_forward(float distance)
{
    state = 2;
    target_distance = distance;
}

struct Action 
{
    int type;
    float target_angle;
    float target_distance;
};


int main(int argc, char** argv)
{
    ros::init(argc, argv, "myturtle_control");
    ros::NodeHandle h;
    pub = h.advertise<geometry_msgs::Twist>("turtle1/cmd_vel", 1000);
    ros::Subscriber sub =
        h.subscribe("/turtle1/pose", 1000, poseCallback);
    ros::Rate loopRate(rate);

    float tx = 8, ty = 8;

    
     queue<Action> q;

    
    q.push({2, 0, 3});
    q.push({1, 72*PI /180, 0});
    q.push({2, 0, 3});
    q.push({1,PI- (36 * PI / 180), 0});
    q.push({2, 0, 3});
    q.push({1,PI+(36 * PI / 180), 0});
    q.push({2, 0, 3});
    q.push({1,-72*PI/180, 0});
    q.push({2,0,3});

    q.push({1,0.0});
    q.push({2,0,3});
    q.push({1, 72*PI /180, 0});
    q.push({2, 0, 3});
    q.push({1,PI- (36 * PI / 180), 0});
    q.push({2, 0, 3});
    q.push({1,PI+(36 * PI / 180), 0});
    q.push({2, 0, 3});
    q.push({1,-72*PI/180, 0});
    q.push({2,0,3});

     

     

    bool in_action = false;
    while (ros::ok()) {
        //pub.publish(getMessage(linear_x, 5.0));
        //linear_x += 1.0;
        if (state == 0 && !in_action) {
            if (q.size() > 0) {
                Action a = q.front();
                q.pop();
                if (a.type == 1) rotate(a.target_angle);
                else if (a.type == 2) straight_forward(a.target_distance);
                in_action = true;
                cout << "state=" << state
                     << " action=" << a.type << " " << a.target_distance << " " << a.target_angle << endl;
            }
        }
        else if (state == 0 && in_action) {
            in_action = false;
        }
        else if (state == 1) handleStateRotate();
        else if (state == 2) handleStateStraightForward();
       loopRate.sleep();
        ros::spinOnce();
    }

    return 0;
}
