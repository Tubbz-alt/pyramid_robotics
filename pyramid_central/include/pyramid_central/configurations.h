#ifndef PYRAMID_CENTRAL_CONFIGURATIONS_H
#define PYRAMID_CENTRAL_CONFIGURATIONS_H

#include <ros/ros.h>

#include "pyramid_central/parameters.h"

namespace system_commander
{

template<typename T> inline void GetRosParameter(const ros::NodeHandle& nh,
                                                     const std::string& key,
                                                     const T& default_value,
                                                     T* value)
{
    ROS_ASSERT(value != nullptr);
    bool have_parameter = nh.getParam(key, *value);
    if (!have_parameter)
    {
        ROS_WARN_STREAM("[rosparam]: could not find parameter " << nh.getNamespace()
        << "/" << key << ", setting to default: " << default_value);
        *value = default_value;
    }
}

inline void GetRotorConfiguration(const ros::NodeHandle& nh,
                                  RotorConfiguration* rotor_configuration)
{
    std::map<std::string, double> single_rotor;
    std::string rotor_configuration_string = "rotor_configuration/";
    unsigned int i = 0;
    while (nh.getParam(rotor_configuration_string + std::to_string(i), single_rotor))
    {
        if (i == 0) {
            rotor_configuration->rotors.clear();
        }
        Rotor rotor;
        nh.getParam(rotor_configuration_string + std::to_string(i) + "/angle",
            rotor.angle);
        nh.getParam(rotor_configuration_string + std::to_string(i) + "/arm_length",
            rotor.arm_length);
        nh.getParam(rotor_configuration_string + std::to_string(i) + "/rotor_force_constant",
            rotor.rotor_force_constant);
        nh.getParam(rotor_configuration_string + std::to_string(i) + "/rotor_moment_constant",
            rotor.rotor_moment_constant);
        nh.getParam(rotor_configuration_string + std::to_string(i) + "/direction",
            rotor.direction);
        rotor_configuration->rotors.push_back(rotor);
        ++i;
    }
}

inline void GetSystemParameters(const ros::NodeHandle& nh, SystemParameters* system_parameters) {
    GetRosParameter(nh, "mass",
        system_parameters->mass_,
        &system_parameters->mass_);
    GetRosParameter(nh, "inertia/xx",
        system_parameters->inertia_(0, 0),
        &system_parameters->inertia_(0, 0));
    GetRosParameter(nh, "inertia/xy",
        system_parameters->inertia_(0, 1),
        &system_parameters->inertia_(0, 1));
        system_parameters->inertia_(1, 0) = system_parameters->inertia_(0, 1);
    GetRosParameter(nh, "inertia/xz",
        system_parameters->inertia_(0, 2),
        &system_parameters->inertia_(0, 2));
        system_parameters->inertia_(2, 0) = system_parameters->inertia_(0, 2);
    GetRosParameter(nh, "inertia/yy",
        system_parameters->inertia_(1, 1),
        &system_parameters->inertia_(1, 1));
    GetRosParameter(nh, "inertia/yz",
        system_parameters->inertia_(1, 2),
        &system_parameters->inertia_(1, 2));
        system_parameters->inertia_(2, 1) = system_parameters->inertia_(1, 2);
    GetRosParameter(nh, "inertia/zz",
        system_parameters->inertia_(2, 2),
        &system_parameters->inertia_(2, 2));
    GetRotorConfiguration(nh, &system_parameters->rotor_configuration_);
}

} //namespace system_commander

#endif //PYRAMID_CENTRAL_CONFIGURATIONS_H