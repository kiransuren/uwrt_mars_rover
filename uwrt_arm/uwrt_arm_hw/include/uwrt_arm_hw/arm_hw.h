/**
Copyright (c) 2019 Somesh Daga <s2daga@uwaterloo.ca>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include "uwrt_arm_hw/voltage_joint_interface.h"

#include <hardware_interface/controller_info.h>
#include <hardware_interface/joint_command_interface.h>
#include <hardware_interface/joint_state_interface.h>
#include <hardware_interface/robot_hw.h>
#include <joint_limits_interface/joint_limits_interface.h>
#include <ros/ros.h>
#include <transmission_interface/transmission_info.h>
#include <urdf/model.h>

#include <list>
#include <map>
#include <string>
#include <vector>

namespace uwrt
{
namespace arm
{
/**
 * Base class for the Arm hardware interface required by ros_control
 */ 
class ArmHW : public hardware_interface::RobotHW
{
public:
  /**
   * \brief Constructor
   * 
   * \param urdf_str URDF of the arm in string format
   */
  explicit ArmHW(const std::string& urdf_str);

  /**
   * \brief: Constructor
   * 
   * \param name Arbitrary name for an instance of this class
   * \param urdf_str URDF of the arm in string format
   */
  ArmHW(const std::string& name, const std::string& urdf_str);

  /**
   * \brief Initializes the hardware interface by registering all joints and interfaces
   * 
   * \param nh Nodehandle for the root of the arm namespace
   * \param arm_hw_nh Nodehandle in the namespace of the arm hardware interface
   */ 
  bool init(ros::NodeHandle& nh,
            ros::NodeHandle& arm_hw_nh) override;
  /**
   * \brief Starts/Stops controllers based on controller manager service calls
   * 
   * \param start_list Controllers to start
   * \param stop_list Controllers to stop
   */
  void doSwitch(const std::list<hardware_interface::ControllerInfo>& start_list,
                const std::list<hardware_interface::ControllerInfo>& stop_list) override;
  /**
   * \brief Reads data from the arm
   * 
   * \param time The current time
   * \param period The time passes since last call to read
   */
  virtual void read(const ros::Time& time, const ros::Duration& period) = 0;
  /**
   * \brief Writes data to the arm
   * 
   * \param time The current time
   * \param period The time passes since last call to write
   */
  virtual void write(const ros::Time& time, const ros::Duration& period) = 0;

  /**
   * \brief Enforces predefined limits on controller outputs
   * 
   * \param period Control period
   */
  void enforceLimits(ros::Duration period);
  /**
   * \brief Get the name of the hardware interface object
   */
  std::string getName() const;

protected:
  /**
   * \brief Enum for the mode of control for a joint
   */
  enum ControlMethod {POSITION, VELOCITY, EFFORT, VOLTAGE};

  ros::NodeHandle nh_;
  ros::NodeHandle arm_hw_nh_;

  const std::string name_;
  const std::string urdf_string_;
  urdf::Model urdf_model_;
  const int8_t num_joints_;
  std::vector<std::string> joint_names_;
  std::vector<double>
    joint_lower_limits_,
    joint_upper_limits_,
    joint_effort_limits_;
  std::vector<double>
    joint_position_,
    joint_position_command_,
    joint_velocity_,
    joint_velocity_command_,
    joint_effort_,
    joint_effort_command_,
    joint_voltage_command_;
  std::vector<ControlMethod> joint_control_method_;
  std::map<std::string, uint8_t> joint_index_map_;  ///< Mapping from joint name to an index
  std::vector<int> joint_types_;

  // Hardware Interfaces
  hardware_interface::JointStateInterface    joint_state_interface_;
  hardware_interface::PositionJointInterface joint_position_interface_;
  hardware_interface::VelocityJointInterface joint_velocity_interface_;
  hardware_interface::EffortJointInterface   joint_effort_interface_;
  hardware_interface::VoltageJointInterface  joint_voltage_interface_;

  // Joint Limit Interfaces
  joint_limits_interface::PositionJointSaturationInterface position_sat_interface_;
  joint_limits_interface::PositionJointSoftLimitsInterface position_limits_interface_;
  joint_limits_interface::VelocityJointSaturationInterface velocity_sat_interface_;
  joint_limits_interface::VelocityJointSoftLimitsInterface velocity_limits_interface_;
  joint_limits_interface::EffortJointSaturationInterface   effort_sat_interface_;
  joint_limits_interface::EffortJointSoftLimitsInterface   effort_limits_interface_;

private:
  void registerJointLimits(const std::string& joint_name,
                           const urdf::Model& urdf_model,
                           const hardware_interface::JointHandle& joint_handle_position,
                           const hardware_interface::JointHandle& joint_handle_velocity,
                           const hardware_interface::JointHandle& joint_handle_effort,
                           double* const joint_lower_limit,
                           double* const joint_upper_limit,
                           double* const joint_effort_limit);
  void registerInterfaces(const urdf::Model& urdf_model);
  // void parseTransmissionsFromURDF();
};  // class ArmHW
}  // namespace arm
}  // namespace uwrt