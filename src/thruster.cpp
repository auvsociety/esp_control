#include "thruster.hpp"

Servo    g_thrusters[NUMBER_OF_THRUSTERS];
int32_t  g_thruster_pins[NUMBER_OF_THRUSTERS] = {15, 2, 16, 5, 19, 23, 27};

uint32_t g_last_thrust_time_ms = 0;

void initialiseThrusters() {
  digitalWrite(2, LOW);

  for (int i = 0; i < NUMBER_OF_THRUSTERS; i++) {
    g_thrusters[i].attach(g_thruster_pins[i]);
  }

  // Write neutral to arm ESCs
  for (int i = 0; i < NUMBER_OF_THRUSTERS; i++) {
    g_thrusters[i].writeMicroseconds(PWM_NEUTRAL);
  }

  // Blink twice on success
  for (int b = 0; b < 2; b++) {
    delay(500); digitalWrite(2, HIGH);
    delay(500); digitalWrite(2, LOW);
  }
}

void killThrusters() {
  for (int i = 0; i < NUMBER_OF_THRUSTERS; i++) {
    g_thrusters[i].writeMicroseconds(PWM_NEUTRAL);
  }
}

void setThrusterThrottle(const int32_t *pwm_values) {
  for (int i = 0; i < NUMBER_OF_THRUSTERS; i++) {
    int32_t pwm = constrain(pwm_values[i], PWM_MIN, PWM_MAX);
    g_thrusters[i].writeMicroseconds(pwm);
  }

  g_last_thrust_time_ms = millis();
}
