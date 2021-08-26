#ifndef DOSQUARE_H
#define DOSQUARE_H

class dosquare
{
private:

public:

ros::Publisher *velocity_publisher;

ros::Subscriber pose_subscriber;
ros::Subscriber range_subscriber;

dosquare();
void odomCallback(const nav_msgs::Odometry::ConstPtr& msg);
void rangeCallback( const sensor_msgs::Range::ConstPtr& msg);
void move(double speed, double distance, bool isForward);
void rotate (double angular_speed, double angle, bool clockwise);
void moveGoal(geometry_msgs::Pose2D  goal_pose, double distance_tolerance,double speed);
void setDesiredOrientation (double desired_angle_radians);
void gridClean();
void spiralClean();
void SetSubscriber();

};

#endif // DOSQUARE_H
