#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "sensor_msgs/msg/range.hpp"
#include "geometry_msgs/msg/pose2_d.h"
#include "tf2/LinearMath/Quaternion.h"
#include  <tf2/transform_datatypes.h>

class testencoder: public rclcpp::Node
{
private:

nav_msgs::msg::Odometry odommsg;
geometry_msgs::msg::Pose2D bot_pose; 
geometry_msgs::msg::Twist vel_msg;
geometry_msgs::msg::Pose2D  pose;
int countdown=20;


rclcpp::TimerBase::SharedPtr timer_;
rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr ptr_pub;
rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr sub;

public:

testencoder() : Node("testencoder")
{
ptr_pub =  this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 100);
sub =this->create_subscription<nav_msgs::msg::Odometry>("odom", 1000, std::bind(&testencoder::odomCallback, this,std::placeholders::_1));
      // set staring position
      pose.x=5;
      pose.y=0;
      pose.theta=0;

      RCLCPP_INFO(this->get_logger(), "\n\n\n******START TESTING************\n");



      //set a random linear velocity in the x-axis
      vel_msg.linear.x =.1;
      vel_msg.linear.y =0;
      vel_msg.linear.z =0;
      //set a random angular velocity in the y-axis
      vel_msg.angular.x = 0;
      vel_msg.angular.y = 0;
      vel_msg.angular.z =0;
      ptr_pub->publish(vel_msg);      

      timer_ = this->create_wall_timer(std::chrono::milliseconds(5), std::bind(&pubpwm::timer_callback, this));


}

void timer_callback()
{
if (countdown >0)
  {
  RCLCPP_INFO(this->get_logger(), "%d\n",countdown);
  }
else
  {
      vel_msg.linear.x =0;
      ptr_pub->publish(vel_msg);  


  RCLCPP_INFO(this->get_logger(), "\n\n\n******End TESTING************\n");

      /** test your code here **/
  RCLCPP_INFO(this->get_logger(), "\n\n\bot pose %lf %lf %lf\n",pose.x,pose.y,pose.theta);
  }

}




void odomCallback(const nav_msgs::msg::Odometry::SharedPtr& msg){
        bot_pose.x=msg->pose.pose.position.x;
        bot_pose.y=msg->pose.pose.position.y;

        tf::Quaternion q(
                    msg->pose.pose.orientation.x,
                    msg->pose.pose.orientation.y,
                    msg->pose.pose.orientation.z,
                    msg->pose.pose.orientation.w
                    );
        bot_pose.theta= tf2::getYaw(q);
odommsg.pose.pose.position.x = msg->pose.pose.position.x;
odommsg.pose.pose.position.y = msg->pose.pose.position.y;
odommsg.twist.twist.linear.x = msg->twist.twist.linear.x;
odommsg.twist.twist.linear.y = msg->twist.twist.linear.y;
odommsg.twist.twist.angular.z = msg->twist.twist.angular.z;
odommsg.twist.twist.linear.z = sqrt(pow(odommsg.twist.twist.linear.y,2 ) + pow(odommsg.twist.twist.linear.x,2));


}

};



int main(int argc, char **argv)
{

  rclcpp::init(argc, argv);

  auto node = std::make_shared<testencoder>();
  rclcpp::spin(node);
  rclcpp::shutdown();







  return 0;
}

