#include "main.hpp"

static uint32_t prevUpdateTime  = 0;
static uint32_t prevPublishTime = 0;

void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  Serial.begin(115200);
  delay(2000);

  Wire.begin(DEPTH_SENSOR_SDA_PIN, DEPTH_SENSOR_SCL_PIN);
  Wire.setClock(100000);

  #ifndef NO_DEPTH_SENSOR
    initializeDepthSensor();
  #endif

  set_microros_serial_transports(Serial);

  initialiseThrusters();     // Arms ESCs at neutral (1500 µs)
  initializeComms();         // Blocks until micro-ROS agent handshakes

  prevUpdateTime  = micros();
  prevPublishTime = millis();
}

void loop() {
  // ── Sensor read at UPDATE_INTERVAL_uS (100 µs = 10 kHz)
  #ifndef NO_DEPTH_SENSOR
    if (micros() - prevUpdateTime >= UPDATE_INTERVAL_uS) {
      prevUpdateTime = micros();
      readDepthSensor();
    }
  #endif

  // ── Publish depth + check comms at PUBLISH_INTERVAL_MS (100 ms)
  if (millis() - prevPublishTime >= PUBLISH_INTERVAL_MS) {
    prevPublishTime = millis();
    sendDepth(depth);
    checkComms();   // Spins executor (processes incoming /control/pwm) + pings agent
  }

  if (millis() - g_last_thrust_time_ms > THRUST_TIMEOUT_MS) {
    killThrusters();
  }
}
