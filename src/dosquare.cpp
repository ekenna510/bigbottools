//#include "dosquare.h"
#include "std_msgs/msg/string.h"
#include <thread>
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "sensor_msgs/msg/range.hpp"
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose2_d.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include  <tf2/transform_datatypes.h>
#include <sstream>

class dosquare: public rclcpp::Node
{
private:

  double speed, angular_speed;
  double distance, angle;
  bool isForward, clockwise;

  rclcpp::Publisher<geometry_msgs::msg::Vector3>::SharedPtr ptr_pub;  
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odomsub;
  rclcpp::Subscription<sensor_msgs::msg::Range>::SharedPtr sensorsub;

private:

nav_msgs::msg::Odometry odommsg;
geometry_msgs::msg::Pose2D  bot_pose;
double frontRange;
bool gotRange=false;

bool isInitialPoseSet = false;



const double x_min = 0.0;
const double y_min = 0.0;
const double x_max = 11.0;
const double y_max = 11.0;
const double PI = 3.14159265359;

// double degrees2radians(double angle_in_degrees);
// double getDistance(double x1, double y1, double x2, double y2);
// double AvgDiff();
// double Trend();


public:




dosquare(): Node("dosquare")        
{
  ptr_pub = this->create_publisher<geometry_msgs::msg::Vector3>("cmd_vel", 100);
  odomsub = this->create_subscription<nav_msgs::msg::Odometry>("odom", 100, std::bind(&dosquare::odomCallback, this,std::placeholders::_1));
  sensorsub = this->create_subscription<sensor_msgs::msg::Range>("sensor", 100, std::bind(&dosquare::rangeCallback, this,std::placeholders::_1));



}
void rangeCallback(const sensor_msgs::msg::Range::SharedPtr msg)
{

   RCLCPP_INFO(this->get_logger(), "Sonar Range: [%f]", msg->range);
   frontRange = msg->range;
   gotRange = true;
}
void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg){
        bot_pose.x=msg->pose.pose.position.x;
        bot_pose.y=msg->pose.pose.position.y;

        tf2::Quaternion q(
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

   RCLCPP_INFO(this->get_logger(), "odomCallback: x %f y %f theta %f", bot_pose.x,bot_pose.y,bot_pose.theta);

}



/**
 *  makes the robot move with a certain linear velocity for a
 *  certain distance in a forward or backward straight direction.
 */
void move(double speed, double distance, bool isForward){
        geometry_msgs::msg::Twist vel_msg;
        //set a random linear velocity in the x-axis
        if (isForward)
                vel_msg.linear.x =fabs(speed);
        else
                vel_msg.linear.x =-fabs(speed);
        vel_msg.linear.y =0;
        vel_msg.linear.z =0;
        //set a random angular velocity in the y-axis
        vel_msg.angular.x = 0;
        vel_msg.angular.y = 0;
        vel_msg.angular.z =0;

        // save current position as start
        geometry_msgs::msg::Pose2D start_pos;
        start_pos.x = bot_pose.x;
        start_pos.y = bot_pose.y;

        RCLCPP_INFO(this->get_logger(), "speed %f distance %f linear X %f",speed,distance,vel_msg.linear.x);

        double t0 = this->now().toSec();
        double current_distance =  getDistance(bot_pose.x, bot_pose.y, start_pos.x, start_pos.y);
//        velocity_publisher->publish(vel_msg);

        rclcpp::Rate loop_rate(10);
        do{
                ptr_pub->publish(vel_msg);
                double t1 = this->now().toSec();
                rclcpp::spinOnce();
                //std::this_thread::sleep_for(
                loop_rate.sleep();
                current_distance =  getDistance(bot_pose.x, bot_pose.y, start_pos.x, start_pos.y);
                RCLCPP_INFO(this->get_logger(), "distance %f current_distance %f gotRange %d range %f",distance, current_distance,gotRange,frontRange);

                //std::cout <<(t1-t0)<<", "<<current_distance <<", "<<distance<< std::endl;
        }while(current_distance<distance && ros::ok() && (!gotRange || (gotRange && frontRange >.5 )));
        vel_msg.linear.x =0;
        ptr_pub->publish(vel_msg);
        for(int i=0; i < 50 ;i++)
        {
            ros::spinOnce();
            loop_rate.sleep();
        }
        RCLCPP_INFO(this->get_logger(), "exit move");

}


void rotate (double angular_speed, double relative_angle, bool clockwise){

        geometry_msgs::Twist vel_msg;
        //set a random linear velocity in the x-axis
        vel_msg.linear.x =0;
        vel_msg.linear.y =0;
        vel_msg.linear.z =0;
        //set a random angular velocity in the y-axis
        vel_msg.angular.x = 0;
        vel_msg.angular.y = 0;

        if (clockwise)
                vel_msg.angular.z =-fabs(angular_speed);
        else
                vel_msg.angular.z =fabs(angular_speed);

        RCLCPP_INFO(this->get_logger(), "angular %f %f",vel_msg.angular.z,angular_speed);
        double current_angle = bot_pose.theta;

        ptr_pub->publish(vel_msg);

        double t0 = this->now().toSec();
        ros::Rate loop_rate(10);
        do{
                ptr_pub->publish(vel_msg);
                double t1 = this->now().toSec();
                //current_angle = angular_speed * (t1-t0);
                ros::spinOnce();
                loop_rate.sleep();
                current_angle = bot_pose.theta;
                RCLCPP_INFO(this->get_logger(), "current_angle %f",current_angle);

        }while( ((!clockwise && current_angle <relative_angle) || (clockwise && current_angle >relative_angle)) && ros::ok());

        vel_msg.angular.z =0;
        ptr_pub->publish(vel_msg);

}

double degrees2radians(double angle_in_degrees){
        return angle_in_degrees *PI /180.0;
}

double getDistance(double x1, double y1, double x2, double y2){
        return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}



void setDesiredOrientation (double desired_angle_radians){
        double relative_angle_radians = desired_angle_radians - bot_pose.theta;//theta;
        bool clockwise = ((relative_angle_radians<0)?true:false);
        //std::cout <<desired_angle_radians <<","<<bot_pose.theta<<","<<relative_angle_radians<<","<<clockwise<< std::endl;
        rotate (degrees2radians(10), fabs(relative_angle_radians), clockwise);

}

void moveGoal(geometry_msgs::msg::Pose2D goal_pose, double distance_tolerance,double speed){

        geometry_msgs::msg::Twist vel_msg;


        bool firstTime = true;
        double thetaRequested;
        double averageRequested;
        double thetaDiff;
        double averageDiff;
        double alphaFactor=.7;


        ros::Rate loop_rate(10);
        double E = speed;
        //linear velocity in the x-axis
        double Kp=1.0;

        double distance;
        do{
                /****** Proportional Controller ******/
                //double v0 = 2.0;
                //double alpha = 0.5;
                distance= getDistance(bot_pose.x, bot_pose.y, goal_pose.x, goal_pose.y);
                if (distance > 3)
                {
                    vel_msg.linear.x = speed;
                }
                else
                {
                    E = (distance/3.0);

                    if (E * speed > odommsg.twist.twist.linear.z )
                    {
                        vel_msg.linear.x =  E * speed;
                    }
                    else
                    {
                        vel_msg.linear.x = odommsg.twist.twist.linear.z;
                    }
                }
                if (vel_msg.linear.x < .1)
                {
                    vel_msg.linear.x = .1;
                }
                vel_msg.linear.y =0;
                vel_msg.linear.z =0;
                //angular velocity in the z-axis
                vel_msg.angular.x = 0;
                vel_msg.angular.y = 0;

                thetaRequested = vel_msg.angular.z;

                thetaDiff = bot_pose.theta - thetaRequested;
                if (firstTime )
                    {
                    averageDiff = thetaDiff;
                    averageRequested = thetaRequested;
                    }
               //https://stackoverflow.com/questions/10990618/calculate-rolling-moving-average-in-c

                averageDiff = (alphaFactor * thetaDiff) + (1.0 - alphaFactor) * averageDiff;
                averageRequested =(alphaFactor * thetaRequested) + (1.0 - alphaFactor) * averageRequested;
                thetaRequested =(atan2(goal_pose.y-bot_pose.y, goal_pose.x-bot_pose.x)-bot_pose.theta);
                vel_msg.angular.z =((fabs(thetaDiff)-fabs(averageDiff)) *.4) +thetaRequested;

                ptr_pub->publish(vel_msg);
                RCLCPP_INFO(this->get_logger(), "thetaDiff %f averageDiff %f thetaRequested %f averageRequested %f",thetaDiff,averageDiff,thetaRequested,averageRequested);
                ros::spinOnce();
                loop_rate.sleep();

        }while(distance >distance_tolerance && ros::ok());
        std::cout<<"end move goal"<< std::endl;
        vel_msg.linear.x =0;
        vel_msg.angular.z = 0;
        ptr_pub->publish(vel_msg);
}



void gridClean(){

        ros::Rate loop(0.5);
        geometry_msgs::msg:Pose2D pose;
        pose.x=1;
        pose.y=1;
        pose.theta=0;
        moveGoal(pose, 0.01,02);
        loop.sleep();
        setDesiredOrientation(0);
        loop.sleep();

        move(2, 9, true);
        loop.sleep();
        rotate(degrees2radians(10), degrees2radians(90), false);
        loop.sleep();
        move(2, 9, true);


        rotate(degrees2radians(10), degrees2radians(90), false);
        loop.sleep();
        move(2, 1, true);
        rotate(degrees2radians(10), degrees2radians(90), false);
        loop.sleep();
        move(2, 9, true);

        rotate(degrees2radians(30), degrees2radians(90), true);
        loop.sleep();
        move(2, 1, true);
        rotate(degrees2radians(30), degrees2radians(90), true);
        loop.sleep();
        move(2, 9, true);


        double distance = getDistance(bot_pose.x, bot_pose.y, x_max, y_max);

}


void spiralClean(){
        geometry_msgs::Twist vel_msg;
        double count =0;

        double constant_speed=4;
        double vk = 1;
        double wk = 2;
        double rk = 0.5;
        ros::Rate loop(1);

        do{
                rk=rk+1.0;
                vel_msg.linear.x =rk;
                vel_msg.linear.y =0;
                vel_msg.linear.z =0;
                //set a random angular velocity in the y-axis
                vel_msg.angular.x = 0;
                vel_msg.angular.y = 0;
                vel_msg.angular.z =constant_speed;//((vk)/(0.5+rk));

                std::cout <<"vel_msg.linear.x = "<<vel_msg.linear.x << std::endl;
                std::cout <<"vel_msg.angular.z = "<<vel_msg.angular.z<< std::endl;
                ptr_pub->publish(vel_msg);
                ros::spinOnce();

                loop.sleep();
                //vk = vel_msg.linear.x;
                //wk = vel_msg.angular.z;
                //rk = vk/wk;
                std::cout <<rk<<", "<<vk <<", "<<wk<< std::endl;
        }while((bot_pose.x<10.5)&&(bot_pose.y<10.5));
        vel_msg.linear.x =0;
        ptr_pub->publish(vel_msg);
}

};



int main(int argc, char **argv)
{

  rclcpp::init(argc, argv); 
  auto node = std::make_shared<dosquare>();
        


  //ros::Rate r(10);


//  while (!isInitialPoseSet)
//  {
//      ros::spinOnce();

//  }

//   while (ros::ok())
//   {

//       RCLCPP_INFO(node->get_logger(), "\n\n\n******START TESTING************\n");
//       geometry_msgs::Pose2D  pose;
//       pose.x=5;
//       pose.y=0;
//       pose.theta=0;

//       doit.move(.3, 1.2, true);
//       RCLCPP_INFO(node->get_logger(), "\n\n\n******after move************\n");
   
      // angular speed,angle, clockwise
// 
//       doit.rotate (.1, 1.5, false);

//       RCLCPP_INFO(node->get_logger(), "\n\n\n******after rotate************\n");
//       doit.move(.1, .5, true);

//       RCLCPP_INFO(node->get_logger(), "\n\n\n******End TESTING************\n"); 

      /** test your code here **/

      /*std::cout <<"enter speed: ";
      cin>>speed;
      std::cout <<"enter distance: ";
      cin>>distance;
      std::cout <<"forward?: ";
      cin>>isForward;
      move(speed, distance, isForward);

      std::cout <<"enter angular velocity (degree/sec): ";
      cin>>angular_speed;
      std::cout <<"enter desired angle (degrees): ";
      cin>>angle;
      std::cout <<"clockwise ?: ";
      cin>>clockwise;
      rotate(degrees2radians(angular_speed), degrees2radians(angle), clockwise);
       */
//       ros::Rate loop_rate(0.5);
      /*setDesiredOrientation(degrees2radians(120));

      loop_rate.sleep();
      setDesiredOrientation(degrees2radians(-60));
      loop_rate.sleep();
      setDesiredOrientation(degrees2radians(0));*/

      /*turtlesim::Pose goal_pose;
      goal_pose.x=1;
      goal_pose.y=1;
      goal_pose.theta=0;
      moveGoal(goal_pose, 0.01);
      loop_rate.sleep();
       */

      //gridClean();

  //    ros::Rate loop(0.5);

//      doit.moveGoal(pose, 0.3,0.1);

 //     pose.x=4;
 //     pose.y=3;
 //     pose.theta=0;
 //     doit.moveGoal(pose, 0.01,0.5);

 //     vel_msg.linear.x =0;
 //     vel_msg.angular.z = 0;
 //     velocity_publisher->publish(vel_msg);


   //   loop.sleep();
      //spiralClean();

   //   ros::spin();

      return 0;
  }



