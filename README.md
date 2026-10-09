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

## Result synchronization

The [operations archive](https://vercel-home-telemetry-api.vercel.app/laboratory.html?project=pid-pressure-control) stores execution results. Supabase migrations are in the [API repository](https://github.com/brunnojob/vercel-home-telemetry-api/tree/main/supabase/migrations).

```sh
python cloud/sync.py enqueue result.json --project pid-pressure-control
python cloud/sync.py sync
```

Set `BRUNNODEV_ACCESS_TOKEN` to your session token. The SQLite outbox retains reports until the server confirms persistence; identical content does not create duplicate records. Tokens are not stored in source code. To run the synchronization tests:

```sh
python -m unittest discover -s cloud
```
