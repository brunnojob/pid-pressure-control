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

Contribution trailer: `Co-authored-by: nyctophile <329826984+ineedfoundmyway@users.noreply.github.com>`.

## Execution proof

[![Executable proof](https://github.com/brunnojob/pid-pressure-control/actions/workflows/proof.yml/badge.svg)](https://github.com/brunnojob/pid-pressure-control/actions/workflows/proof.yml)

[Recorded execution and downloadable evidence](https://github.com/brunnojob/pid-pressure-control/actions/workflows/proof.yml)

Run `python .proof/record.py` after installing the prerequisites above. The scenarios execute repository code and verify exit codes and expected output. CI publishes `execution-proof` with the transcript, input fingerprints and source commit. The downloadable report identifies the exact tested version; the workflow badge tracks the latest run.
