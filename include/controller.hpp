#pragma once

#include "pid.hpp"

#include <cstdint>
#include <optional>

enum class LoopStatus { Ready, SensorFault, StaleSample };

struct Calibration {
    double rawMin;
    double rawMax;
    double pressureMin;
    double pressureMax;
};

struct LoopOutput {
    LoopStatus status;
    double pressure;
    double valvePercent;
};

class PressureLoop {
public:
    PressureLoop(Calibration calibration, PidConfig pid, double setpoint, std::uint64_t maxAgeMs);
    LoopOutput sample(std::optional<double> raw, std::uint64_t timestampMs, std::uint64_t nowMs, double dtSeconds);
    void reset();

private:
    double calibrate(double raw) const;
    Calibration calibration_;
    PidController pid_;
    double setpoint_;
    std::uint64_t maxAgeMs_;
    bool faultLatched_ = false;
};
