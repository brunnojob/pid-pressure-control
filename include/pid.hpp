#pragma once

#include <optional>

struct PidConfig {
  double kp;
  double ki;
  double kd;
  double outputMin;
  double outputMax;
  double derivativeTau;
  double integralLimit;
};

struct PidState {
  double integral;
  double filteredDerivative;
  double previousMeasurement;
  bool initialized;
};

class PidController {
public:
  explicit PidController(PidConfig config);
  double update(double setpoint, double measurement, double dtSeconds);
  void reset();
  const PidState &state() const;

private:
  PidConfig config_;
  PidState state_{0.0, 0.0, 0.0, false};
};
