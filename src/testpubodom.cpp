#include <string>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose2_d.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include <tf2_ros/transform_broadcaster.h>
#include <tf2/LinearMath/Quaternion.h>

class testpubodom: public rclcpp::Node
{

private:

rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr ptr_pub;
rclcpp::TimerBase::SharedPtr timer_;
nav_msgs::msg::Odometry msg;    
geometry_msgs::msg::Pose2D bot_pose; // hold current position of bot.
    int dt;
    double  deltaLeft, deltaRight, Velth, deltaX, deltaY,deltath,linear, angular;
public:
testpubodom(): Node("testpubodom")
{
ptr_pub=this->create_publisher<nav_msgs::msg::Odometry>("odom", 100);
timer_ = this->create_wall_timer(std::chrono::milliseconds(5), std::bind(&testpubodom::timer_callback, this));
    bot_pose.x = 0.0;
    bot_pose.y = 0.0;
    bot_pose.theta = 0.0;

    dt = 100;

    // how many click did wee see.
    deltaLeft =  .05;
    deltaRight = .05;

}
void timer_callback()
{


    bot_pose.x += deltaLeft;
    bot_pose.y += deltaRight;
    bot_pose.theta = 0;

    msg.pose.pose.position.x= bot_pose.x;
    msg.pose.pose.position.y= bot_pose.y;
    msg.pose.pose.position.z= 0;


    //---------------------------------------------------------------------------------------
    /*
    http://answers.ros.org/question/11973/gathering-wheel-encoder-data-and-publishing-on-the-odom-topic/
    */
    //geometry_msgs::Quaternion odom_quat = tf2::createQuaternionMsgFromYaw(bot_pose.theta);

    //first, we'll publish the transform over tf
    //geometry_msgs::TransformStamped odom_trans;
    //odom_trans.header.stamp = ros::Time::now();
    //odom_trans.header.frame_id = "odom";
    //odom_trans.child_frame_id = base_frame;

    //odom_trans.transform.translation.x = bot_pose.x;
    //odom_trans.transform.translation.y = bot_pose.y;
    //odom_trans.transform.translation.z = 0.0;
    //odom_trans.transform.rotation = odom_quat;

    //send the transform
//    ptr_broadcaster->sendTransform(odom_trans);

    //next, we'll publish the odometry message over ROS 
  tf2::Quaternion q;
  q.setRPY(0.0, 0.0, bot_pose.theta );

  msg.pose.pose.orientation.x = q.x();
  msg.pose.pose.orientation.y = q.y();
  msg.pose.pose.orientation.z = q.z();
  msg.pose.pose.orientation.w = q.w();



    //  nav_msgs::Odometry odom;
    msg.header.stamp = this->now();//CurrentEncoderTime;
    msg.header.frame_id = "odom";


    //set the position
    msg.pose.pose.position.x = bot_pose.x;
    msg.pose.pose.position.y = bot_pose.y;
    msg.pose.pose.position.z = 0.0;
//    msg.pose.pose.orientation = odom_quat;

    //set the velocity
    msg.child_frame_id = "base_link";
    msg.twist.twist.linear.x = deltaX; //vx;//vx
    msg.twist.twist.linear.y = deltaY;//vy
    msg.twist.twist.angular.z = Velth;

    ptr_pub->publish(msg);


}
};
int main(int argc, char** argv) 
{
    rclcpp::init(argc, argv);    
    auto node = std::make_shared<testpubodom>();
    rclcpp::spin(node);
    rclcpp::shutdown();  
        
}
