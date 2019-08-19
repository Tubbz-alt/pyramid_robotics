#ifndef PYRAMID_CENTRAL_SYSTEM_COMMANDER_NODE_H
#define PYRAMID_CENTRAL_SYSTEM_COMMANDER_NODE_H

#include <ros/ros.h>
#include <Eigen/Core>
#include <pyramid_msgs/default_topics.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/JointState.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/WrenchStamped.h>

#include "pyramid_central/common.h"
#include "pyramid_central/system_commander.h"

namespace system_commander
{

class SystemCommanderNode
{
public:
    SystemCommanderNode(const ros::NodeHandle& nh, const ros::NodeHandle& private_nh);
    ~SystemCommanderNode();

    void InitializeParams();
    void sendTensions(const Eigen::VectorXd& tensions);
    void sendThrust(const Eigen::VectorXd& thrust);

private: //member data
    //general
    ros::NodeHandle nh_;
    ros::NodeHandle private_nh_;

    //topic
    sensor_msgs::JointState tensions_msg;
    geometry_msgs::WrenchStamped thrust_msg;

    //class
    SystemCommander system_commander_;

    //subscriber
    ros::Subscriber trajectory_sub_;
    ros::Subscriber odometry_sub_;
    ros::Subscriber imu_sub_;

    //publisher
    ros::Publisher tensions_pub_;
    ros::Publisher thrust_pub_;

private: //member function
    void DesiredTrajectoryCB(const trajectory_msgs::MultiDOFJointTrajectoryPtr& trajectory_msg);
    void FeedbackOdometryCB(const nav_msgs::OdometryPtr& odometry_msg);
    //void FeedbackImuCB(const sensor_msgs::ImuPtr& imu_msg);

};

} //namespace system_commander

#endif //PYRAMID_CENTRAL_SYSTEM_COMMANDER_NODE_H