#include "sensors.hpp"

MS5837 sensor;
float  depth = 0.0f;

void initializeDepthSensor() {
  // Block until MS5837 initialises (I2C must be up before calling this)
  while (!sensor.init()) {
    delay(5000);
  }

  sensor.setFluidDensity(FLUID_DENSITY_KGM3);

  // Blink once on success
  delay(500); digitalWrite(2, HIGH);
  delay(500); digitalWrite(2, LOW);
}

void readDepthSensor() {
  sensor.read();
  depth = sensor.depth();
}
