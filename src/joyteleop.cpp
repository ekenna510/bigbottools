#include "rclcpp/rclcpp.hpp"
//#include <turtlesim/Velocity.h>
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/vector3.hpp"
#include <sensor_msgs/msg/joy.hpp>

// note on plain values:
// buttons are either 0 or 1
// button axes go from 0 to -1
// stick axes go from 0 to +/-1

#define PS3_BUTTON_SELECT            0
#define PS3_BUTTON_STICK_LEFT        1
#define PS3_BUTTON_STICK_RIGHT       2
#define PS3_BUTTON_START             3
#define PS3_BUTTON_CROSS_UP          4
#define PS3_BUTTON_CROSS_RIGHT       5
#define PS3_BUTTON_CROSS_DOWN        6
#define PS3_BUTTON_CROSS_LEFT        7
#define PS3_BUTTON_REAR_LEFT_2       8
#define PS3_BUTTON_REAR_RIGHT_2      9
#define PS3_BUTTON_REAR_LEFT_1       10
#define PS3_BUTTON_REAR_RIGHT_1      11
#define PS3_BUTTON_ACTION_TRIANGLE   12
#define PS3_BUTTON_ACTION_CIRCLE     13
#define PS3_BUTTON_ACTION_CROSS      14
#define PS3_BUTTON_ACTION_SQUARE     15
#define PS3_BUTTON_PAIRING           16

#define PS3_AXIS_STICK_LEFT_LEFTWARDS    0
#define PS3_AXIS_STICK_LEFT_UPWARDS      1
#define PS3_AXIS_STICK_RIGHT_LEFTWARDS   2
#define PS3_AXIS_STICK_RIGHT_UPWARDS     3
#define PS3_AXIS_BUTTON_CROSS_UP         4
#define PS3_AXIS_BUTTON_CROSS_RIGHT      5
#define PS3_AXIS_BUTTON_CROSS_DOWN       6
#define PS3_AXIS_BUTTON_CROSS_LEFT       7
#define PS3_AXIS_BUTTON_REAR_LEFT_2      8
#define PS3_AXIS_BUTTON_REAR_RIGHT_2     9
#define PS3_AXIS_BUTTON_REAR_LEFT_1      10
#define PS3_AXIS_BUTTON_REAR_RIGHT_1     11
#define PS3_AXIS_BUTTON_ACTION_TRIANGLE  12
#define PS3_AXIS_BUTTON_ACTION_CIRCLE    13
#define PS3_AXIS_BUTTON_ACTION_CROSS     14
#define PS3_AXIS_BUTTON_ACTION_SQUARE    15
#define PS3_AXIS_ACCELEROMETER_LEFT      16
#define PS3_AXIS_ACCELEROMETER_FORWARD   17
#define PS3_AXIS_ACCELEROMETER_UP        18
#define PS3_AXIS_GYRO_YAW                19


class joyteleop : public rclcpp::Node
{



protected:
  //  void joycallback(const sensor_msgs::msg::Joy::SharedPtr joy);

    // configure  axis index
    int linear_axes, angular_axes;
   
    // config action button index
    int linearlock,angularlock,linearunlock,angularunlock;
    int deadman_button;
    
    // orientation
    bool invert_linear=false,invert_angular=false;
   
    
    double maxlinear=0,maxangular=0;
    double l_scale_, a_scale_;
    int countsticky;

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr ptr_pub;

    rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr sub;  

    bool linearsticky = false;
    bool angularsticky = false;
    bool deadman_on = false;

    bool linearset = false;
    bool angularset = false;
    geometry_msgs::msg::Twist vel;

public:

joyteleop() : Node("joyteleop")
{

 linear_axes=PS3_AXIS_STICK_LEFT_UPWARDS;
 angular_axes=PS3_AXIS_STICK_RIGHT_LEFTWARDS;
 deadman_button=6;
 linearlock=5;
 angularlock=7;
 linearunlock=2;
 angularunlock=3;


  this->declare_parameter<int>("linear_axes",linear_axes);
  this->get_parameter("linear_axes",linear_axes);  
  this->declare_parameter<int>("angular_axes",angular_axes);
  this->get_parameter("angular_axes",angular_axes);  

   this->declare_parameter<int>("deadman_button",deadman_button);
  this->get_parameter("deadman_button",deadman_button);  

  this->declare_parameter<int>("linearlock",linearlock);
  this->get_parameter("linearlock",linearlock);  
  this->declare_parameter<int>("angularlock",angularlock);
  this->get_parameter("angularlock",angularlock);  

  this->declare_parameter<int>("invert_linear",invert_linear);
  this->get_parameter("invert_linear",invert_linear);   
  this->declare_parameter<int>("invert_angular",invert_angular);
  this->get_parameter("invert_angular",invert_angular);   
  this->declare_parameter<int>("scale_angular",a_scale_);
  this->get_parameter("scale_angular",a_scale_);   
  this->declare_parameter<int>("linear_axes",l_scale_);
  this->get_parameter("scale_linear",l_scale_);   

ptr_pub  = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 100);
sub =  this->create_subscription<sensor_msgs::msg::Joy>("Joy", 10, std::bind(&joyteleop::joycallback, this, std::placeholders::_1));
int a=0,b=0;
if (invert_linear)
	{
	a =1;
	}
if (invert_angular)
	{
	b=1;
	} 
  RCLCPP_INFO(this->get_logger(), "linear_axes %d angular_axes %d deadman_button %d linearlock %d angularlock %d linearunlock %d angularunlock %d invert_linear %d invert_angular %d scale_angular %f  scale_linear %f ",linear_axes,angular_axes,deadman_button,linearlock,angularlock,linearunlock,angularunlock,a,b,a_scale_,l_scale_);

}
// require dead button
// allow sticky values for stick values


void joycallback(const sensor_msgs::msg::Joy::SharedPtr joy)
{

  double newlinear,newanglar;
  //make sure deadman is pressed
  deadman_on = joy->buttons[deadman_button];
  // calculate new values
  newlinear = l_scale_ * joy->axes[linear_axes];
  newanglar = a_scale_ * joy->axes[angular_axes];

  // inver sign if needed to match forward and left position.
  if (invert_linear)
   {
	newlinear *= -1; 
   }

  if (invert_angular)
   {
   newanglar *= -1;
   }

  // ignore anything too small because joystick does not center well
  if (fabs(newanglar) < .05)
  {
     newanglar = 0;
  }
  if (fabs(newlinear) < .05)
  {
     newlinear = 0;
  }

  // adjust minimums based on both values
  if (fabs(newanglar) > 0 && fabs(newanglar) < 0.2  && fabs(newlinear) < 0.1)
    {
        if (newanglar > 0)
        {
        newanglar = .2;
        }
        else
        {
        newanglar = -.2;
        }
    }


  if (fabs(newlinear) > 0 && fabs(newlinear) < 0.1 && fabs(newanglar) < .2 )
    {
        if (newlinear > 0)
        {
        newlinear = 0.1;
        }
        else
        {
            newlinear = -0.1;
        }
    }   

  if (joy->buttons[linearunlock]> 0)
   {
   RCLCPP_INFO(this->get_logger(), "\n****** linear clear sticky ************\n");
   linearsticky = false;
   linearset = false;
   }

  if (joy->buttons[angularunlock]> 0)
   {
   RCLCPP_INFO(this->get_logger(), "\n****** angular clear sticky ************\n");

   angularsticky = false;
   angularset = false;
   }


  if (joy->buttons[linearlock]> 0 && !linearsticky )
   {
   RCLCPP_INFO(this->get_logger(), "\n****** set linear sticky ************\n");
   linearsticky = true;
   linearset = false;
   countsticky++;
   }

  if (linearsticky)
   {
   if (!linearset)
     {
      maxlinear = newlinear;
      linearset = true;     
      RCLCPP_INFO(this->get_logger(), "\n****** set linearset %f ************\n",maxlinear);

     } 
   }

  //     vel.linear.x =maxlinear;
  if (joy->buttons[angularlock]> 0 && !angularsticky )
   {
   RCLCPP_INFO(this->get_logger(), "\n****** set angular sticky ************\n");

   angularsticky = true;
   angularset = false;
   countsticky++;
   }

  if (angularsticky)
   {
   if (!angularset)
     {
     maxangular = newanglar;
     angularset = true;
     RCLCPP_INFO(this->get_logger(), "\n****** set linearset %f ************\n",maxangular);

     }
   }

  if (deadman_on)
  {
    if (linearsticky)
    {
        vel.linear.x = maxlinear;
    }
    else
    {
        vel.linear.x = newlinear;
    }
    if (angularsticky)
    {
        vel.angular.z = maxangular;
    }
    else
    {
        vel.angular.z = newanglar;
    }
  }
  else
  {
      vel.linear.x = 0;
      vel.angular.z = 0;
  }
  if (vel.linear.x > 0 || vel.linear.x < 0 || vel.angular.z > 0 || vel.angular.z < 0 )
    {
    RCLCPP_INFO(this->get_logger(), "linear %f angular %f",vel.linear.x,vel.angular.z);
    }
  ptr_pub->publish(vel);
}
};
int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);  
  auto node = std::make_shared<joyteleop>();  

  rclcpp::spin(node);
  rclcpp::shutdown();
}
