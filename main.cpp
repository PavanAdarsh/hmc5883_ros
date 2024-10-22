#include <Arduino.h>
#include <ros.h>
#include <Wire.h>
#include<Servo.h>
#include "std_msgs/Int64.h"
#include "geometry_msgs/Vector3.h"

const int slaveadd=0x1E;
int dt=100;
int outpin=3;
int x,value;
Servo myservo;
float mfx,mfy,mfz;

ros::NodeHandle nh;
geometry_msgs::Vector3 magnetic_field;
//std_msgs::Int64 pwmval;

ros::Publisher chatter_pub1("mf",&magnetic_field);

int bldc(const std_msgs::Int64&pwm);

ros::Subscriber <std_msgs::Int64> sub("yaw",&bldc);

void setup() 
{
  //Serial.begin(57600);
  // delay(dt);
  // while(!Serial)
  // {
  //   delay(1);
  // }

  // Serial.println("hello");

  pinMode(outpin,OUTPUT);
  myservo.attach(outpin);
  myservo.writeMicroseconds(1478);
  delay(100);


  Wire.begin();
  Wire.beginTransmission(slaveadd);
  Wire.write(0x00);
  Wire.write(0x70);
  Wire.endTransmission(true);

  Wire.beginTransmission(slaveadd);
  Wire.write(0x01);
  Wire.write(0xA0);
  Wire.endTransmission(true);

  Wire.beginTransmission(slaveadd);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission(true);

  nh.initNode();
  nh.advertise(chatter_pub1);
  nh.subscribe(sub);

}

void loop() 
{
  Wire.beginTransmission(slaveadd);
  Wire.write(0x03);
  Wire.endTransmission(false);
  Wire.requestFrom(slaveadd,6,true);

  if(Wire.available())//if we have 6 bytes to be read
  {
    magnetic_field.x=(Wire.read()<<8|Wire.read()); //msb value is shifted left
    magnetic_field.z=(Wire.read()<<8|Wire.read());
    magnetic_field.y=(Wire.read()<<8|Wire.read());
  }
  else
  {
    //Serial.println("data not available");
  }

  chatter_pub1.publish(&magnetic_field);

  // Serial.print("x:");
  // Serial.println(magnetic_field.x);
  // Serial.print("y:");
  // Serial.println(magnetic_field.y);
  // Serial.print("z:");
  // Serial.println(magnetic_field.z);

  nh.spinOnce();
  delay(dt);

}

int bldc(const std_msgs::Int64& pwmval)
{
  value=pwmval.data;
  //value=1700;
  x=map(value,1000,2000,1100,1900);//0 corresponds to full speed in one dirn, 180 for opp, 90 for stop
  
  //Serial.println(x);
  myservo.writeMicroseconds(x);  
}
