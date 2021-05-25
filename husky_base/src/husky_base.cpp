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

#include "controller_manager/controller_manager.h"
#include "husky_base/horizon_legacy/ThunderBorgCythonMock.h"
#include "husky_base/husky_hardware.h"
#include "ros/callback_queue.h"
#include <Python.h>
#include <boost/chrono.hpp>
#include <ros/console.h>

typedef boost::chrono::steady_clock time_source;

/**
 * Control loop for Husky, not realtime safe
 */
void controlLoop(husky_base::HuskyHardware &husky,
                 controller_manager::ControllerManager &cm,
                 time_source::time_point &last_time) {

  // Calculate monotonic time difference
  time_source::time_point this_time = time_source::now();
  boost::chrono::duration<double> elapsed_duration = this_time - last_time;
  ros::Duration elapsed(elapsed_duration.count());
  last_time = this_time;

  // Process control loop
  husky.reportLoopDuration(elapsed);
  husky.updateJointsFromHardware();
  cm.update(ros::Time::now(), elapsed);
  husky.writeCommandsToHardware();
}

/**
 * Diagnostics loop for Husky, not realtime safe
 */
void diagnosticLoop(husky_base::HuskyHardware &husky) {
  husky.updateDiagnostics();
}

int main(int argc, char *argv[]) {
  ros::init(argc, argv, "husky_base");
  ros::NodeHandle nh, private_nh("~");

  double control_frequency, diagnostic_frequency;
  private_nh.param<double>("control_frequency", control_frequency, 10.0);
  private_nh.param<double>("diagnostic_frequency", diagnostic_frequency, 1.0);

  // Initialize robot hardware and link to controller manager
  husky_base::HuskyHardware husky(nh, private_nh, control_frequency);
  controller_manager::ControllerManager cm(&husky, nh);

  // Setup separate queue and single-threaded spinner to process timer callbacks
  // that interface with Husky hardware - libhorizon_legacy not threadsafe. This
  // avoids having to lock around hardware access, but precludes realtime
  // safetyb in the control loop.
  ros::CallbackQueue husky_queue;
  ros::AsyncSpinner husky_spinner(1, &husky_queue);

  time_source::time_point last_time = time_source::now();
  ros::TimerOptions control_timer(ros::Duration(1 / control_frequency),
                                  boost::bind(controlLoop, boost::ref(husky),
                                              boost::ref(cm),
                                              boost::ref(last_time)),
                                  &husky_queue);
  ros::Timer control_loop = nh.createTimer(control_timer);

  ros::TimerOptions diagnostic_timer(
      ros::Duration(1 / diagnostic_frequency),
      boost::bind(diagnosticLoop, boost::ref(husky)), &husky_queue);
  ros::Timer diagnostic_loop = nh.createTimer(diagnostic_timer);

  // Initialize cython-thunderborg
  Py_SetProgramName(argv[0]); /* optional but recommended */
  Py_Initialize();

  // import_ThunderBorgCythonMock();

  // ThunderBorgCythonMock::ThunderBorg tb;

  // std::string path = "/home/ubuntu/catkin_ws/devel/lib/husky_base/";
  //   PyObject* pyModuleName = PyUnicode_DecodeFSDefault(path);
  //  PyObject *pName = PyString_FromString("ThunderBorgCythonMock.pyx");
  // PySys_SetPath("/home/ubuntu/catkin_ws/devel/lib/husky_base/");

  // PyObject *sys_path = PySys_GetObject("path");
  // PyList_Append(sys_path, PyString_FromString(
  //                            "/home/ubuntu/catkin_ws/devel/lib/husky_base/"));
  // PyObject *pmodule = PyImport_ImportModule("ThunderBorgCythonMock");

  // if (!pmodule) {
  //   PyErr_Print();
  //   //     // fprintf(stderr, "Error: could not import module 'embedded'\n");
  //   //     // ROS_INFO("Embedding Failed");
  //   ros::shutdown();
  //   return 1;
  // }

  //  PyInit_ThunderBorgCythonMock();

  initThunderBorgCythonMock();

  c_ThunderBorg *borg = buildThunderBorg();

  std::cout << SetMotor1Wrapper(borg, 1.0) << std::endl;
  // c_Thunderborg_t tb;
  //  ThunderBorg tb;
  //  c_ThunderBorg tb;
  //  tb.c_member = 1;

  husky_spinner.start();

  // Process remainder of ROS callbacks separately, mainly ControlManager
  // related
  ros::spin();

  /* Clean up after using CPython. */
  // PyMem_RawFree(argv[0]);
  Py_Finalize();

  return 0;
}
