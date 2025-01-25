# Linefollower Robot (Raspberry Pi Pico)

This repository contains a line-follower robot project for the **Raspberry Pi Pico** (or Pico W). The code is split into multiple C files:

- **linefollower.c** — Contains the `main()` function, plus PID loop logic.  
- **sensors.h / sensors.c** — Handles sensor reading (QTRX reflectance array), analog multiplexers, and calibration routines.  
- **motors.h / motors.c** — Manages motor pins (PWM + direction), motor initialization, and speed control.  
- **CMakeLists.txt** — The build configuration for CMake.

---

## 1. Prerequisites

1. **Visual Studio Code**  
2. **CMake** (3.13 or later)  
3. **Ninja** (or Make) — used by CMake to compile.  
4. **Arm GCC Toolchain** (e.g., `arm-none-eabi-gcc`) — required for building Pico projects.  
5. **Pico SDK** — Download or clone from [github.com/raspberrypi/pico-sdk](https://github.com/raspberrypi/pico-sdk). Ensure `PICO_SDK_PATH` is set or the VS Code extension knows where the SDK is located.
