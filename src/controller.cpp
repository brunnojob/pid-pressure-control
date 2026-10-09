#include "controller.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

PressureLoop::PressureLoop(Calibration calibration, PidConfig pid,
                           double setpoint, std::uint64_t maxAgeMs)
    : calibration_(calibration), pid_(pid), setpoint_(setpoint),
      maxAgeMs_(maxAgeMs) {
  if (calibration_.rawMin >= calibration_.rawMax ||
      calibration_.pressureMin >= calibration_.pressureMax || maxAgeMs_ == 0)
    throw std::invalid_argument("invalid_sensor_calibration");
}

LoopOutput PressureLoop::sample(std::optional<double> raw,
                                std::uint64_t timestampMs, std::uint64_t nowMs,
                                double dtSeconds) {
  if (faultLatched_)
    return {LoopStatus::SensorFault, 0.0, 0.0};
  if (!raw || !std::isfinite(*raw) || *raw < calibration_.rawMin ||
      *raw > calibration_.rawMax) {
    faultLatched_ = true;
    return {LoopStatus::SensorFault, 0.0, 0.0};
  }
  if (timestampMs > nowMs || nowMs - timestampMs > maxAgeMs_) {
    faultLatched_ = true;
    return {LoopStatus::StaleSample, 0.0, 0.0};
  }
  const double pressure = calibrate(*raw);
  const double valve = pid_.update(setpoint_, pressure, dtSeconds);
  return {LoopStatus::Ready, pressure, valve};
}

void PressureLoop::reset() {
  faultLatched_ = false;
  pid_.reset();
}

double PressureLoop::calibrate(double raw) const {
  const double ratio =
      (raw - calibration_.rawMin) / (calibration_.rawMax - calibration_.rawMin);
  return calibration_.pressureMin +
         ratio * (calibration_.pressureMax - calibration_.pressureMin);
}
