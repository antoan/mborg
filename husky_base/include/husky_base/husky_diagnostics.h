/**
 *
 *  \author     Paul Bovbel <pbovbel@clearpathrobotics.com>
 *  \copyright  Copyright (c) 2014-2015, Clearpath Robotics, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Clearpath Robotics, Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL CLEARPATH ROBOTICS, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Please send comments, questions, or patches to code@clearpathrobotics.com
 *
 */

#ifndef HUSKY_BASE_HUSKY_DIAGNOSTICS_H
#define HUSKY_BASE_HUSKY_DIAGNOSTICS_H

#include "diagnostic_updater/diagnostic_updater.h"
#include "husky_base/horizon_legacy_wrapper.h"
#include "husky_msgs/HuskyStatus.h"
#include "ros/ros.h"

namespace husky_base {

class HuskySoftwareDiagnosticTask : public diagnostic_updater::DiagnosticTask {

public:
  explicit HuskySoftwareDiagnosticTask(husky_msgs::HuskyStatus &msg,
                                       double target_control_freq);

  void run(diagnostic_updater::DiagnosticStatusWrapper &stat);

  void updateControlFrequency(double frequency);

  void reportControlVelocities(double angular_vel_request_left,
                               double angular_vel_request_right,
                               double throttle_left, double throttle_right,
                               double throttle_limited_left,
                               double throttle_limited_right);

private:
  void reset();

  double control_freq_, target_control_freq_;

  double angular_vel_request_left_;
  double angular_vel_request_right_;
  double throttle_left_, throttle_right_;
  double throttle_limited_left_;
  double throttle_limited_right_;

  husky_msgs::HuskyStatus &msg_;
};

class HuskyBatteryDiagnosticTask : public diagnostic_updater::DiagnosticTask {

public:
  explicit HuskyBatteryDiagnosticTask(husky_msgs::HuskyStatus &msg);

  void run(diagnostic_updater::DiagnosticStatusWrapper &stat);

  void updateBatteryStatus(float voltage);

private:
  float battery_voltage_;

  husky_msgs::HuskyStatus &msg_;
};

// template<typename T>
// class HuskyHardwareDiagnosticTask :
//   public diagnostic_updater::DiagnosticTask
// {
// public:
//   explicit HuskyHardwareDiagnosticTask(husky_msgs::HuskyStatus &msg);

//   void run(diagnostic_updater::DiagnosticStatusWrapper &stat)
//   {
//     typename horizon_legacy::Channel<T>::Ptr latest =
//     horizon_legacy::Channel<T>::requestData(1.0); if (latest)
//     {
//       update(stat, latest);
//     }
//   }

//   void update(diagnostic_updater::DiagnosticStatusWrapper &stat, typename
//   horizon_legacy::Channel<T>::Ptr &status);

// private:
//   husky_msgs::HuskyStatus &msg_;
// };

// template<>
// HuskyHardwareDiagnosticTask<clearpath::DataSystemStatus>::HuskyHardwareDiagnosticTask(husky_msgs::HuskyStatus
// &msg);

// template<>
// HuskyHardwareDiagnosticTask<clearpath::DataPowerSystem>::HuskyHardwareDiagnosticTask(husky_msgs::HuskyStatus
// &msg);

// template<>
// HuskyHardwareDiagnosticTask<clearpath::DataSafetySystemStatus>::HuskyHardwareDiagnosticTask(
//   husky_msgs::HuskyStatus &msg);

// template<>
// void HuskyHardwareDiagnosticTask<clearpath::DataSystemStatus>::update(
//   diagnostic_updater::DiagnosticStatusWrapper &stat,
//   horizon_legacy::Channel<clearpath::DataSystemStatus>::Ptr &status);

// template<>
// void HuskyHardwareDiagnosticTask<clearpath::DataPowerSystem>::update(
//   diagnostic_updater::DiagnosticStatusWrapper &stat,
//   horizon_legacy::Channel<clearpath::DataPowerSystem>::Ptr &status);

// template<>
// void HuskyHardwareDiagnosticTask<clearpath::DataSafetySystemStatus>::update(
//   diagnostic_updater::DiagnosticStatusWrapper &stat,
//   horizon_legacy::Channel<clearpath::DataSafetySystemStatus>::Ptr &status);

} // namespace husky_base
#endif // HUSKY_BASE_HUSKY_DIAGNOSTICS_H
