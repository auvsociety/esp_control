#pragma once
#include <Arduino.h>

// Comment this out when a real MS5837 depth sensor is connected
// #define NO_DEPTH_SENSOR

#define NUMBER_OF_THRUSTERS 7

extern int32_t g_thruster_pins[NUMBER_OF_THRUSTERS];

#define UPDATE_INTERVAL_uS   100

#define PUBLISH_INTERVAL_MS  100

#define THRUST_TIMEOUT_MS    500

#define FLUID_DENSITY_KGM3   997

#define PWM_MIN  1100
#define PWM_MAX  1900
#define PWM_NEUTRAL 1500
