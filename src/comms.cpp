#include "comms.hpp"

rcl_publisher_t    depthPub;
rcl_subscription_t thrusterSub;

static int32_t                           _thruster_buf[NUMBER_OF_THRUSTERS];
static std_msgs__msg__MultiArrayDimension _dim_buf[1];

std_msgs__msg__Int32MultiArray thrusterMsg;
std_msgs__msg__Float32         depthMsg;

rclc_executor_t executor;
rclc_support_t  support;
rcl_allocator_t allocator;
rcl_node_t      node;
rcl_timer_t     timer;

void error_loop() {
  while (1) {
    digitalWrite(2, HIGH); delay(250);
    digitalWrite(2, LOW);  delay(250);
  }
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  RCLC_UNUSED(timer);
}

void throttleCb(const void *msgin) {
  const std_msgs__msg__Int32MultiArray *pwm_msg =
      (const std_msgs__msg__Int32MultiArray *)msgin;

  size_t count = (pwm_msg->data.size < (size_t)NUMBER_OF_THRUSTERS)
                     ? pwm_msg->data.size
                     : (size_t)NUMBER_OF_THRUSTERS;

  int32_t pwm_values[NUMBER_OF_THRUSTERS];

  for (size_t i = 0; i < NUMBER_OF_THRUSTERS; i++) {
    pwm_values[i] = PWM_NEUTRAL;
  }
  for (size_t i = 0; i < count; i++) {
    pwm_values[i] = pwm_msg->data.data[i];
  }

  setThrusterThrottle(pwm_values);
}

void initializeComms() {
  while (RMW_RET_OK != rmw_uros_ping_agent(100, 1)) {
    delay(10);
  }

  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "AUV_ESP32", "", &support));

  RCCHECK(rclc_publisher_init_best_effort(
      &depthPub,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
      "/sensors/depth"));

  RCCHECK(rclc_subscription_init_default(
      &thrusterSub,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32MultiArray),
      "/control/pwm"));

  thrusterMsg.data.data     = _thruster_buf;
  thrusterMsg.data.capacity = NUMBER_OF_THRUSTERS;
  thrusterMsg.data.size     = NUMBER_OF_THRUSTERS;

  thrusterMsg.layout.dim.data     = _dim_buf;
  thrusterMsg.layout.dim.capacity = 1;
  thrusterMsg.layout.dim.size     = 1;

  thrusterMsg.layout.dim.data[0].size   = NUMBER_OF_THRUSTERS;
  thrusterMsg.layout.dim.data[0].stride = NUMBER_OF_THRUSTERS;

  for (size_t i = 0; i < NUMBER_OF_THRUSTERS; i++) {
    _thruster_buf[i] = PWM_NEUTRAL;
  }

  RCCHECK(rclc_timer_init_default(
      &timer,
      &support,
      RCL_MS_TO_NS(100),
      timer_callback));

  RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
  RCCHECK(rclc_executor_add_subscription(
      &executor, &thrusterSub, &thrusterMsg, &throttleCb, ON_NEW_DATA));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  // Blink three times to signal successful comms init
  for (int b = 0; b < 3; b++) {
    delay(300); digitalWrite(2, HIGH);
    delay(300); digitalWrite(2, LOW);
  }
}

void sendDepth(float depth_value) {
  depthMsg.data = depth_value;
  RCSOFTCHECK(rcl_publish(&depthPub, &depthMsg, NULL));
}

void checkComms() {
  RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10)));

  // Ping the micro-ROS agent; restart if unreachable
  if (RMW_RET_OK != rmw_uros_ping_agent(100, 1)) {
    // Tear down cleanly before restart
    RCSOFTCHECK(rcl_publisher_fini(&depthPub, &node));
    RCSOFTCHECK(rcl_subscription_fini(&thrusterSub, &node));
    ESP.restart();
  }
}
