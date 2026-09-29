# PID Pressure Control

C++ pressure loop lab with sensor calibration, anti-windup PID regulation, actuator limits and fault latching.

## Build

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/pressure_demo
```

The controller is a deterministic simulation. Validate tuning and safety behavior before any hardware use.
