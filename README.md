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

Export a JSON report from the command above, then run `python cloud/sync.py enqueue result.json --project pid-pressure-control` and `python cloud/sync.py sync`. Synchronization requires `BRUNNODEV_ACCESS_TOKEN` and the external operations API; the local outbox retains unacknowledged reports.

## License

Original source and documentation are MIT licensed; see [LICENSE](LICENSE). Third-party dependencies and media retain their respective terms. Maintained by [Brunno Dev](https://brunnodev.store).
