#include <Arduino.h>
#include <ros.h>
#include <Wire.h>
#include<Servo.h>
#include "std_msgs/Int64.h"
#include "std_msgs/Bool.h"
#include "geometry_msgs/Vector3.h"

const int slaveadd=0x1E;
int dt=50;
int outpin=3;
int x,value;
Servo myservo;
float mfx,mfy,mfz;
long i=0;

geometry_msgs::Vector3 magnetic_field;
std_msgs::Int64 pwmval;

ros::NodeHandle nh;
void bldc(const std_msgs::Int64& pwmval); 
ros::Publisher chatter_pub1("mf",&magnetic_field);
ros::Subscriber<std_msgs::Int64> sub("pwm", &bldc);
void setup() 
{
  Serial.begin(9600);
  while(!Serial)
  {
    delay(1);
  }
  
  pinMode(outpin,OUTPUT);
  myservo.attach(outpin);
  myservo.writeMicroseconds(1478);
  delay(100);
  Wire.setSDA(PB7);
  Wire.setSCL(PB6);
  Wire.begin();
  Wire.beginTransmission(slaveadd);
  Wire.write(0x00);
  Wire.write(0x78);
  Wire.endTransmission(true);

  Wire.beginTransmission(slaveadd);
  Wire.write(0x01);
  Wire.write(0xA0);  
  Wire.endTransmission(true);

  Wire.beginTransmission(slaveadd);
  Wire.write(0x02);
  Wire.write(0xFC);   
  Wire.endTransmission(true);

  Wire.beginTransmission(slaveadd);
  Wire.write(0x03);
  Wire.endTransmission(false);


  nh.initNode();
  nh.advertise(chatter_pub1);
  nh.subscribe(sub);

}

void loop() 
{

  Wire.requestFrom(slaveadd,6,true);
  if(Wire.available())//if we have 6 bytes to be read
  {
    magnetic_field.x=(Wire.read()<<8|Wire.read()); //msb value is shifted left
    magnetic_field.z=(Wire.read()<<8|Wire.read());
    magnetic_field.y=(Wire.read()<<8|Wire.read());
  }
  else
  {
    Serial.println("data not available");
  }

  chatter_pub1.publish(&magnetic_field);

  Serial.print("x:");
  Serial.println(magnetic_field.x);
  Serial.print("y:");
  Serial.println(magnetic_field.y);
  Serial.print("z:");
  Serial.println(magnetic_field.z);
  delay(dt);
  i++;
  if(i==1000)
  i=0;
  nh.spinOnce();

}

void bldc(const std_msgs::Int64& pwmval)
{
  value=pwmval.data;
  x=map(value,1000,2000,1100,1900);//0 corresponds to full speed in one dirn, 180 for opp, 90 for stop
  myservo.writeMicroseconds(x);  
}


