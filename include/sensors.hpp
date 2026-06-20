#pragma once
#include "globals.hpp"
#include "MS5837.h"

extern MS5837 sensor;
extern float  depth;

void initializeDepthSensor();
void readDepthSensor();
