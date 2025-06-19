// BSD 3-Clause License
//
// Copyright (c) 2023, Northwestern University MSR
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: Nick Morales

#include <string>
#include "rclcpp/rclcpp.hpp"

// RAII node to terminate the indicated process
class RemoteProcessHandler : public rclcpp::Node
{
public:
  RemoteProcessHandler()
  : Node("remote_process_handler"),
  // get screen process name to kill
  screen_process_name_{declare_parameter<std::string>("screen_process_name")},
  // Construct the command to send Ctrl-C to the screen session
  interrupt_command_{"screen -S " + screen_process_name_ + " -X stuff ^C"},
  // Construct the command to gracefully quit the screen session
  quit_command_{"screen -S " + screen_process_name_ + " -X quit"}
  {
  }

  ~RemoteProcessHandler()
  {
    // Attempt to gracefully stop the process by sending Ctrl-C
    RCLCPP_INFO(this->get_logger(), "Sending Ctrl-C to the process...");
    system(interrupt_command_.c_str());

    // Wait for the process to exit (you may want to implement a timeout)
    RCLCPP_INFO(this->get_logger(), "Waiting for the process to finish...(6 seconds)");
    std::this_thread::sleep_for(std::chrono::seconds(60)); // Adjust the wait time if necessary

    // After waiting, close the screen session
    RCLCPP_INFO(this->get_logger(), "Closing the screen session...");
    system(quit_command_.c_str());
  }

  const std::string screen_process_name() const
  {
    return screen_process_name_;
  }
  
private:
  const std::string screen_process_name_ = "";
  const std::string interrupt_command_ = "";
  const std::string quit_command_ = "";
};

int main(int argc, char * argv[])
{

  // Init ROS
  rclcpp::init(argc, argv);

  auto node = std::make_shared<RemoteProcessHandler>();

  try
  {
    while (rclcpp::ok()) {
      rclcpp::spin_some(node);
    }
    RCLCPP_INFO_STREAM(node->get_logger(), "\nTERMINATING: " << node->screen_process_name());

    // Shutdown ROS
    rclcpp::shutdown();
  }
  catch (const rclcpp::exceptions::RCLError & e)
  {
  }

  return 0;
}