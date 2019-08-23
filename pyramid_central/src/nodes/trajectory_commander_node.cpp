#include "trajectory_commander_node.h"

namespace system_commander
{

TrajectoryCommanderNode::TrajectoryCommanderNode(
    const ros::NodeHandle& nh, const ros::NodeHandle& private_nh)
    :nh_(nh),
     private_nh_(private_nh),
     desired_position_(Eigen::Vector3d::Zero()),
     desired_yaw_(0.0)
{
    trajectory_pub_ = nh_.advertise<trajectory_msgs::MultiDOFJointTrajectory>
                        (pyramid_msgs::default_topics::COMMAND_TRAJECTORY, 10);

    //set up dynamic reconfigure
    srv_ = boost::make_shared <dynamic_reconfigure::Server<pyramid_central::SystemCommanderConfig>>( private_nh);
    dynamic_reconfigure::Server<pyramid_central::SystemCommanderConfig>::CallbackType cb
        = boost::bind(&TrajectoryCommanderNode::SetTrajectoryCB, this, _1, _2);
    srv_->setCallback(cb);
}

TrajectoryCommanderNode::~TrajectoryCommanderNode(){ }

void TrajectoryCommanderNode::SetTrajectoryCB(pyramid_central::SystemCommanderConfig &config,
                                              uint32_t level)
{
    system_reconfigure_.TrajectoryReconfig(config);

    trajectory_msg.header.stamp = ros::Time::now();

    // Default desired position and yaw.
    desired_position_ = system_reconfigure_.getDesiredPosition();
    desired_yaw_ = system_reconfigure_.getDesiredYaw();

    mav_msgs::msgMultiDofJointTrajectoryFromPositionYaw(desired_position_,
                                                        desired_yaw_, &trajectory_msg);

    ROS_INFO("Publishing waypoint on namespace %s: [%f, %f, %f].",
             nh_.getNamespace().c_str(), desired_position_.x(),
             desired_position_.y(), desired_position_.z());

    trajectory_pub_.publish(trajectory_msg);
}

} //namespace system_commander

int main(int argc, char** argv) {
    ros::init(argc, argv, "trajectory_commander_node");
    ros::NodeHandle nh;
    ros::NodeHandle private_nh("~");

    ROS_INFO("Started trajectory_commander_node.");
    /*
    std_srvs::Empty srv;
    bool unpaused = ros::service::call("/gazebo/unpause_physics", srv);
    unsigned int i = 0;

    // Trying to unpause Gazebo for 10 seconds.
    while (i <= 10 && !unpaused) {
        ROS_INFO("Wait for 1 second before trying to unpause Gazebo again.");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        unpaused = ros::service::call("/gazebo/unpause_physics", srv);
        ++i;
    }

    if (!unpaused) {
        ROS_FATAL("Could not wake up Gazebo.");
        return -1;
    } else {
        ROS_INFO("Unpaused the Gazebo simulation.");
    }
    */
    // Wait for 5 seconds to let the Gazebo GUI show up.
    ros::Duration(5.0).sleep();

    system_commander::TrajectoryCommanderNode trajectory_commander_node(nh, private_nh);

    ros::spin();

    return 0;
}
