#include "sensors.hpp"

MS5837 sensor;
float  depth = 0.0f;

void initializeDepthSensor() {
#ifdef DEPTH_SENSOR_DEBUG
  bool foundSensor = false;
  Serial.printf("Scanning I2C (SDA=%d, SCL=%d)...\n",
                DEPTH_SENSOR_SDA_PIN, DEPTH_SENSOR_SCL_PIN);
  for (uint8_t address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.printf("I2C device found at 0x%02X\n", address);
      foundSensor = true;
    }
  }
  if (!foundSensor) {
    Serial.println("No I2C devices found; MS5837 should be at 0x76.");
  }
#endif

  // Keep retrying so the sensor is ready before depth is published.
  while (!sensor.init(Wire)) {
#ifdef DEPTH_SENSOR_DEBUG
    Serial.println("MS5837 init failed (expected I2C address 0x76); retrying in 5 s.");
#endif
    delay(5000);
  }

#ifdef DEPTH_SENSOR_DEBUG
  Serial.println("MS5837 initialized successfully.");
#endif
  sensor.setFluidDensity(FLUID_DENSITY_KGM3);

  // Blink once on success
  delay(500); digitalWrite(2, HIGH);
  delay(500); digitalWrite(2, LOW);
}

void readDepthSensor() {
  sensor.read();
  depth = sensor.depth();
}
