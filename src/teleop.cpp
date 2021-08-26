#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <signal.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>

#define KEYCODE_R 0x43 
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71
#define KEYCODE_S 0x53
#define KEYCODE_s 0x73
#define KEYCODE_F 0x46
#define KEYCODE_f 0x66
#define KEYCODE_G 0x47
#define KEYCODE_g 0x67

#define KEYCODE_H 0x48
#define KEYCODE_h 0x68
#define KEYCODE_I 0x49
#define KEYCODE_i 0x69



class TeleopTurtle: public rclcpp::Node
{

  //TeleopTurtle();
  //void keyLoop();

private:

  geometry_msgs::msg::Twist twist;
  double linear_, angular_, l_scale_, a_scale_;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr ptr_pub;
  int kfd = 0;
  struct termios cooked, raw;

public:

TeleopTurtle(): Node("teleop_turtle")
{
  linear_=0;
  angular_=0;
  l_scale_=0.1;
  a_scale_=0.4;  
  this->declare_parameter<int>("scale_angular",a_scale_);
  this->get_parameter("scale_angular",a_scale_);  
  this->declare_parameter<int>("scale_linear",l_scale_);
  this->get_parameter("scale_linear",l_scale_);  

  ptr_pub = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 1);  
  //signal(SIGINT,TeleopTurtle::quit);

}



void quit(int sig)
{
  (void)sig;
  tcsetattr(kfd, TCSANOW, &cooked);
  //ros::shutdown();
  exit(0);
}


void keyLoop()
{
  char c;
  bool dirty=false;


  // get the console in raw mode                                                              
  tcgetattr(kfd, &cooked);
  memcpy(&raw, &cooked, sizeof(struct termios));
  raw.c_lflag &=~ (ICANON | ECHO);
  // Setting a new line, then end of file                         
  raw.c_cc[VEOL] = 1;
  raw.c_cc[VEOF] = 2;
  tcsetattr(kfd, TCSANOW, &raw);

  puts("Reading from keyboard");
  puts("---------------------------");
  puts("L left R Right U up D down S stop F lin faster by .1 G ang faster by .1");
  puts("H lin slower by .1 I ang slower by .1");
  puts("Use arrow keys to move the turtle. Q quit");


  for(;;)
  {
    // get the next event from the keyboard  
    if(read(kfd, &c, 1) < 0)
    {
      perror("read():");
      exit(-1);
    }

    linear_=angular_=0;
    RCLCPP_DEBUG(this->get_logger(), "value: 0x%02X\n", c);
  
    switch(c)
    {
      case KEYCODE_L:
        RCLCPP_INFO(this->get_logger(), "LEFT");
        angular_ = 1.0;
        dirty = true;
        break;
      case KEYCODE_R:
        RCLCPP_INFO(this->get_logger(), "RIGHT");
        angular_ = -1.0;
        dirty = true;
        break;
      case KEYCODE_U:
        RCLCPP_INFO(this->get_logger(), "UP");
        linear_ = 1.0;
        dirty = true;
        break;
      case KEYCODE_D:
        RCLCPP_INFO(this->get_logger(), "DOWN");
        linear_ = -1.0;
        dirty = true;
        break;
      case KEYCODE_S:
        RCLCPP_INFO(this->get_logger(), "Stop");
        linear_ = 0.0;
        dirty = true;
        break;
      case KEYCODE_s:
        RCLCPP_INFO(this->get_logger(), "Stop");
        linear_ = 0.0;
        dirty = true;
        break;
      case KEYCODE_F:
		l_scale_ =l_scale_+ .01;
        RCLCPP_INFO(this->get_logger(), "Fast linear %0.4f",l_scale_);
        dirty = true;
        break;		
      case KEYCODE_f:
		l_scale_ =l_scale_+ .01;
        RCLCPP_INFO(this->get_logger(), "Fast linear %0.4f",l_scale_);
        dirty = true;
        break;		
      case KEYCODE_G:
		a_scale_ = a_scale_+ .01;		
        RCLCPP_INFO(this->get_logger(), "Fast angular %0.4f",a_scale_);
        dirty = true;
        break;		
      case KEYCODE_g:
		a_scale_ = a_scale_+ .01;		
        RCLCPP_INFO(this->get_logger(), "Fast angular %0.4f",a_scale_);
        dirty = true;		
        break;
      case KEYCODE_H:
		l_scale_ = l_scale_- .01;
        RCLCPP_INFO(this->get_logger(), "Slow linear %0.4f",l_scale_);
        dirty = true;
        break;		
      case KEYCODE_h:
		l_scale_ = l_scale_- .01;
        RCLCPP_INFO(this->get_logger(), "Slow linear %0.4f",l_scale_);
        dirty = true;
        break;		
      case KEYCODE_I:
		a_scale_ = a_scale_- .01;		
        RCLCPP_INFO(this->get_logger(), "Slow angular %0.4f",a_scale_);
        dirty = true;
        break;		
      case KEYCODE_i:
		a_scale_ = a_scale_- .01;		
        RCLCPP_INFO(this->get_logger(), "Slow angular %0.4f",a_scale_);
        dirty = true;		
        break;
      case KEYCODE_Q:
        TeleopTurtle::quit(SIGINT);
        break;

    }
   


    twist.angular.z = a_scale_*angular_;
    twist.linear.x = l_scale_*linear_;
    if(dirty ==true)
    {
      ptr_pub->publish(twist);    
      dirty=false;
    }
  }


  return;
}
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
 
  auto node = std::make_shared<TeleopTurtle>();

  node->keyLoop();
  
  return(0);
}


