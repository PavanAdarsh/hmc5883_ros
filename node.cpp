#include "ros/ros.h"
#include <cmath>
#include "geometry_msgs/Vector3.h"
#include "std_msgs/Int64.h"

const double pi = 3.14159265359;

float mx,my,mz,y,pwm;
std_msgs::Int64 value;

void m(const geometry_msgs::Vector3::ConstPtr & M)
{
	mx=M->x;
	my=M->y;
	mz=M->z;
	//std::cout<<mx<<std::endl;
	//std::cout<<my<<std::endl;
	//std::cout<<mz<<std::endl;
}


int yaw(float mx,float my,float mz)//yaw value, output as pwm
{
	y=atan2(my,mx);
	//std::cout<<y<<std::endl;
	pwm=((y+pi)*1000)/(2*pi)+1000;//map function from -pi,pi to 1000,2000
	return pwm;
}

int main(int argc,char **argv)
{
	ros::init(argc,argv,"node");
	ros::NodeHandle n;
	ros::Subscriber sub = n.subscribe("mf",50,m);

	ros::Publisher chatter_pub = n.advertise<std_msgs::Int64>("yaw",50);
	
	while(ros::ok())
	{
		pwm=yaw(mx,my,mz);
		//std::cout<<pwm;
		//pwm = 1100;
		value.data=pwm;
		//std::cout<<value;
		chatter_pub.publish(value);
		
		ros::spinOnce();
	}
return 0;	
}
	
	
	
