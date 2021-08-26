
// ROS includes
#include "rclcpp/rclcpp.hpp"

#include <sensor_msgs/msg/laser_scan.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <signal.h>

class fake_scan: public rclcpp::Node
{
private:
rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr ptr_pub;

sensor_msgs::msg::LaserScan CDLaser_msg;
int counts;
double maxd;
int midpoint;
double rangeinterval;
rclcpp::TimerBase::SharedPtr timer_;

public:

fake_scan() : Node("fake_scan")
{
    ptr_pub=this->create_publisher<sensor_msgs::msg::LaserScan>("scan", 100);    

    maxd =14;
    counts = 100;
    midpoint = counts/2;
    rangeinterval = maxd/midpoint;

    CDLaser_msg.ranges.resize(counts);
    CDLaser_msg.intensities.resize(counts);
    CDLaser_msg.header.frame_id = "laser_frame";
    CDLaser_msg.angle_min = -3.14;
    CDLaser_msg.angle_max = 3.14;
    CDLaser_msg.angle_increment = (CDLaser_msg.angle_max - CDLaser_msg.angle_min) / (double)counts;
    CDLaser_msg.range_min = 0.08;
    CDLaser_msg.range_max = 16.0;
    CDLaser_msg.time_increment =.0005; // seconds between measurements
    CDLaser_msg.scan_time = .1; // seconds between scans


  timer_ = this->create_wall_timer(std::chrono::milliseconds(5), std::bind(&fake_scan::timer_callback, this));

}
void timer_callback()
{




for (int i=0;i < counts; i++)
    {
    CDLaser_msg.ranges[i] = maxd - (fabs(midpoint - i )*rangeinterval) ;
    CDLaser_msg.intensities[i] = .5;
    }
CDLaser_msg.header.stamp = this->now();
ptr_pub->publish(CDLaser_msg);
}

};
//

// double f_angle_min;
// double f_angle_max;
// double f_angle_increment;
// double f_time_increment;
// double f_scan_time;
// double f_range_min;
// double f_range_max;

// int publisher_timer;



int main(int argc, char * argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<fake_scan>();


  rclcpp::spin(node);
  rclcpp::shutdown();

}
