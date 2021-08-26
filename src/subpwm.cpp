#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/vector3.hpp"

class subpwm : public rclcpp::Node
{

protected:
public:

//rclcpp::Subscription<geometry_msgs::msg:Vector3>::SharedPtr sub;
rclcpp::Subscription<geometry_msgs::msg::Vector3>::SharedPtr sub;


subpwm() : Node("subpwm")
{
   
  RCLCPP_INFO(this->get_logger(), "subpwm started");
  sub = this->create_subscription<geometry_msgs::msg::Vector3>("pwm", 1000, std::bind(&subpwm::pwmCallback, this,std::placeholders::_1));


}


/**
* This tutorial demonstrates simple receipt of messages over the ROS system.
*/
void pwmCallback(const geometry_msgs::msg::Vector3::SharedPtr msg)
{

   RCLCPP_INFO(this->get_logger(), "I heard: left %f right %f reset %f", msg->x,msg->y,msg->z);
 

}

};
int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */

    rclcpp::init(argc, argv);
    auto node = std::make_shared<subpwm>();
    rclcpp::spin(node);
    rclcpp::shutdown();


  /**
   * The subscribe() call is how you tell ROS that you want to receive messages
   * on a given topic.  This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing.  Messages are passed to a callback function, here
   * called chatterCallback.  subscribe() returns a Subscriber object that you
   * must hold on to until you want to unsubscribe.  When all copies of the Subscriber
   * object go out of scope, this callback will automatically be unsubscribed from
   * this topic.
   *
   * The second parameter to the subscribe() function is the size of the message
   * queue.  If messages are arriving faster than they are being processed, this
   * is the number of messages that will be buffered up before beginning to throw
   * away the oldest ones.
   */

  /**
   * ros::spin() will enter a loop, pumping callbacks.  With this version, all
   * callbacks will be called from within this thread (the main one).  ros::spin()
   * will exit when Ctrl-C is pressed, or the node is shutdown by the master.
   */


   return 0;
}
