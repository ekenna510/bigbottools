#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/vector3.hpp"

class pubpwm: public rclcpp::Node
{
protected:
 int delay = 10;
 int refreshrate = 5;
 int startLeft = 100;
 int startRight = 0;
 int endLeft = 250;
 int endRight = 0;
 int delaycounter;
 int incrLeft = 1;
 int incrRight = 1;
 geometry_msgs::msg::Vector3 pwm;
 rclcpp::TimerBase::SharedPtr timer_;
 rclcpp::Publisher<geometry_msgs::msg::Vector3>::SharedPtr ptr_pub;

public:

void  setArguments(int pstartLeft,int pstartRight,int pendLeft,int pendRight,int pdelay)
{
  startLeft = pstartLeft;
  startRight = pstartRight;
  endLeft = pendLeft;
  endRight = pendRight;
  delay = pdelay;
  if (startLeft > endLeft) 
    {
      incrLeft = -1;
    }
  else if (endLeft > startLeft)
  {
    incrLeft = 1;
  }
    else
  {
    incrLeft=0;
  }
  if (startRight > endRight) 
    {
      incrRight = -1;
    }
  else if (endRight >startRight  )
  {
    incrRight = 1;
  }
    else
  {
    incrRight=0;
  }
 

  // override with new values
  pwm.x= startLeft;  // left pwm
  pwm.y = startRight;  // right pwm
  pwm.z = 0;   // sent reset encoder value
  delaycounter =delay* refreshrate;
  ptr_pub->publish(pwm);  
}
pubpwm() : Node("pubpwm")
{
  RCLCPP_INFO(this->get_logger(), "pubpwm starting");  

  ptr_pub = this->create_publisher<geometry_msgs::msg::Vector3>("pwm", 100);

  if (startLeft > endLeft) 
    {
      incrLeft = -1;
    }
  else if (endLeft > startLeft)
  {
    incrLeft = 1;
  }
    else
  {
    incrLeft=0;
  }
  if (startRight > endRight) 
    {
      incrRight = -1;
    }
  else if (endRight >startRight  )
  {
    incrRight = 1;
  }
    else
  {
    incrRight=0;
  }
 
  // set these to default
  pwm.x= startLeft;  // left pwm
  pwm.y = startRight;  // right pwm
  pwm.z = 0;   // sent reset encoder value



  timer_ = this->create_wall_timer(std::chrono::milliseconds(5), std::bind(&pubpwm::timer_callback, this));


}

//MotorControl::timer_callback
void timer_callback()
{


pwm.z = 0;

  delaycounter--;
  if (delaycounter <= 0)
  {
    delaycounter =delay* refreshrate;
    //pwm.z = 1; // sent reset encoder value
    // increment left pwm if it did not start at 0

    pwm.x +=incrLeft;

    // increment right pwm if it did not start at 0

    pwm.y +=incrRight;

    if (incrLeft > 0)
    {
    if (pwm.x > endLeft )
    {
      pwm.x=0;
      pwm.y=0;
      pwm.z=0;
      incrLeft=0;
    }
    }
    else
    {
      if (pwm.x < endLeft)
      {
        pwm.x=0;
        pwm.y=0;
        pwm.z=0;
        incrLeft=0;
      }
    }
    if (incrRight > 0)
    {
    if (pwm.x > endRight )
    {
      pwm.x=0;
      pwm.y=0;
      pwm.z=0;
      incrRight=0;
    }
    }
    else
    {
      if (pwm.x < endRight)
      {
        pwm.x=0;
        pwm.y=0;
        pwm.z=0;
        incrRight=0;
      }
    }

    ptr_pub->publish(pwm);
    RCLCPP_INFO(this->get_logger(), "pubpwm pub %lf %lf %lf",pwm.x,pwm.y,pwm.z);

  }
}


};


int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  int startLeft = 100;
  int startRight = 0;
  int endLeft = 250;
  int endRight = 0;
  int delay = 10;
  //size_t pubcount;


  auto node = std::make_shared<pubpwm>();
  //pubcount = node->count_publishers("pwm");
  if (argc >= 6)
    {
    startLeft  = atoi(argv[1]);
    startRight  = atoi(argv[2]);
    endLeft  = atoi(argv[3]);
    endRight  = atoi(argv[4]);
    delay = atoi(argv[5]);
    node->setArguments(startLeft,startRight,endLeft,endRight,delay);
    }

    rclcpp::spin(node);
    rclcpp::shutdown();

   
    return 0;

}



