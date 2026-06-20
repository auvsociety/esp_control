#pragma once
#include "globals.hpp"
#include <ESP32Servo.h>

extern Servo    g_thrusters[NUMBER_OF_THRUSTERS];

extern uint32_t g_last_thrust_time_ms;

void setThrusterThrottle(const int32_t *pwm_values);
void initialiseThrusters();
void killThrusters();
