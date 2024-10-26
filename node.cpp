#include "ros/ros.h"
#include <cmath>
#include "geometry_msgs/Vector3.h"
#include "std_msgs/Int64.h"

const double pi = 3.14159265359;

float mx, my, mz, yaw = 0, pwm = 0;
std_msgs::Int64 pwm_msg;
bool mag_status = false;

void calculate_pwm()
{
    yaw = atan2(-my, mx) * (180 / pi); // Yaw in degrees
    pwm = ((yaw + 180) * 1000) / 360 + 1000; // Map -180 to 180 degrees to 1000-2000
}

void m(const geometry_msgs::Vector3::ConstPtr &M)
{
    mx = M->x;
    my = M->y;
    mz = M->z;
    calculate_pwm();
    mag_status = true;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "node");
    ros::NodeHandle n;
    ros::Publisher pwm_pub = n.advertise<std_msgs::Int64>("pwm", 100); 
    ros::Subscriber sub = n.subscribe("mf", 10, m);                  
    while (ros::ok())
    {
        if (mag_status)
        {
            pwm_msg.data = pwm;
            pwm_pub.publish(pwm_msg);
            ROS_INFO("yaw: %f", yaw);
            mag_status = false;
        }
        ros::spinOnce();
    }
    return 0;
}

