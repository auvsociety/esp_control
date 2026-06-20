# AMU_Control_V2 — ESP32 Thruster Bridge

Firmware for the ESP32 that drives thrusters via PWM and reads an depth sensor, bridged to ROS 2 over micro-ROS (serial transport).

Talks to [**pecka_tvmc**](https://github.com/auvsociety/pecka_tvmc) (ROS 2 Humble).

---

## Topics

| Topic | Type | Direction |
|---|---|---|
| `/control/pwm` | `std_msgs/Int32MultiArray` | Subscribed — PWM µs values from `PwmBridgeNode` |
| `/sensors/depth` | `std_msgs/Float32` | Published — depth in metres |

---

## Build and flash

```bash
# Check build size without flashing
pio run

# Flash to a connected ESP32
pio run --target upload
```

Set the correct port in `platformio.ini` if auto-detect picks the wrong device:
```ini
upload_port = /dev/ttyUSB0
```

### Finding the correct port

```bash
lsusb                     # confirm the ESP32's USB-serial chip is detected
dmesg | tail -20          # check kernel driver messages
ls /dev/tty*              # find the new device (usually /dev/ttyUSB0 or /dev/ttyACM0)
```

If the port is detected but permission is denied:
```bash
sudo usermod -aG dialout $USER
# log out and back in (or reboot) for this to apply
```

---

## Running the micro-ROS agent (required before pecka_tvmc can talk to the ESP32)

The agent bridges the ESP32's serial micro-ROS transport to the ROS 2 graph.

**One-time setup** (separate workspace, e.g. `~/microros_ws`):
```bash
mkdir -p ~/microros_ws/src
cd ~/microros_ws
git clone -b humble https://github.com/micro-ROS/micro_ros_setup.git src/micro_ros_setup
rosdep update
rosdep install --from-paths src --ignore-src -y
colcon build
source install/setup.bash
ros2 run micro_ros_setup create_agent_ws.sh
ros2 run micro_ros_setup build_agent.sh
source install/setup.bash
```

**Every time you want to connect the ESP32:**
```bash
source ~/microros_ws/install/setup.bash
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0 -b 115200
```

Wait for the agent to report a successful connection once the ESP32 boots and handshakes (`Serial.begin(115200)` in `main.cpp` must match the `-b` baud rate above).

---

## Verifying the link

In a separate terminal, after the agent is running and pecka_tvmc is launched:
```bash
ros2 topic echo /sensors/depth
```
If values are printing (even `0.0` with no sensor attached, see below), the ESP32 ↔ agent ↔ ROS 2 chain is alive.

---

## Testing without a depth sensor attached

`initializeDepthSensor()` blocks forever (`while (!sensor.init()) delay(5000);`) if no MS5837 is wired up on I2C. To bench-test without it, define `NO_DEPTH_SENSOR` in `globals.hpp` and guard the sensor calls in `main.cpp` — see the project's bug-fix notes for the exact patch. `depth` will stay at `0.0` and still publish correctly.

---

## Known fixes already applied in this firmware

- Watchdog (`THRUST_TIMEOUT_MS`) now correctly compares against an elapsed-time threshold instead of an always-true condition — thrusters previously killed themselves every loop iteration
- PWM values are clamped to `[1100, 1900]` before being written to the ESC
- `/control/pwm` callback bounds-checks the incoming array length
- `timer_callback` no longer overwrites the real depth value with an incrementing counter
- Static buffers replace `malloc()` for the `Int32MultiArray` message — no heap churn
- Fluid density is a single `#define FLUID_DENSITY_KGM3` in `globals.hpp` (997 = freshwater, 1029 = seawater)

See `include/globals.hpp` for all tunable constants (`THRUST_TIMEOUT_MS`, `PWM_MIN`, `PWM_MAX`, `PWM_NEUTRAL`, `FLUID_DENSITY_KGM3`).