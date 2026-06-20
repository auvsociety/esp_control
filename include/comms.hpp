#pragma once
#include "globals.hpp"
#include "thruster.hpp"
#include "sensors.hpp"

#include <micro_ros_platformio.h>
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>
#include <std_msgs/msg/int32_multi_array.h>

#if !defined(MICRO_ROS_TRANSPORT_ARDUINO_SERIAL)
#endif

extern rcl_publisher_t    depthPub;
extern rcl_subscription_t thrusterSub;

extern std_msgs__msg__Int32MultiArray thrusterMsg;
extern std_msgs__msg__Float32         depthMsg;

extern rclc_executor_t executor;
extern rclc_support_t  support;
extern rcl_allocator_t allocator;
extern rcl_node_t      node;
extern rcl_timer_t     timer;

#define RCCHECK(fn) \
  { rcl_ret_t _rc = (fn); if (_rc != RCL_RET_OK) { error_loop(); } }

#define RCSOFTCHECK(fn) \
  { rcl_ret_t _rc = (fn); (void)_rc; }

void error_loop();
void throttleCb(const void *msgin);
void initializeComms();
void sendDepth(float depth);
void checkComms();
