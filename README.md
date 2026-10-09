# Pressure Control

A PID controller with interlocks and first-order model identification from process measurements.

## Run

Requirements: C++20 and CMake.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
build/identify_process processo.csv 0.1 > result.json
```

## Behavior

Identification estimates the pole, gain, time constant, offset, and residual error. Input contains `input,output` samples plus a sampling interval in seconds. Insufficient data and unstable models are rejected. Use on physical equipment requires validation and equipment-specific tuning.

## Optional report archive

Use the [native C operations archive client](https://github.com/brunnojob/vercel-home-telemetry-api/tree/main/clients/c) to queue `result.json` under project `pid-pressure-control`. The client uses `BRUNNODEV_ACCESS_TOKEN` and retains unacknowledged reports locally.

## License

Original source and documentation are MIT licensed; see [LICENSE](LICENSE). Third-party dependencies and media retain their respective terms. Maintained by [Brunno Dev](https://brunnodev.store).

## Implementation update

Calibration and setpoint values must be finite, with the setpoint inside the engineering range. Stale samples retain the fault until explicit reset. Native regression checks are in `tests/runtime_regressions.cpp`.

Contribution trailer: `Co-authored-by: nyctophile <33561761+ineedfoundmyway@users.noreply.github.com>`.
