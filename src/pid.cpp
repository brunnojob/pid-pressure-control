#include "pid.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

PidController::PidController(PidConfig config) : config_(config) {
    if (config_.outputMin >= config_.outputMax || config_.derivativeTau < 0.0 || config_.integralLimit < 0.0)
        throw std::invalid_argument("invalid_pid_limits");
}

double PidController::update(double setpoint, double measurement, double dtSeconds) {
    if (!std::isfinite(setpoint) || !std::isfinite(measurement) || !std::isfinite(dtSeconds) || dtSeconds <= 0.0 || dtSeconds > 2.0)
        throw std::invalid_argument("invalid_pid_sample");
    const double error = setpoint - measurement;
    const double rawDerivative = state_.initialized ? -(measurement - state_.previousMeasurement) / dtSeconds : 0.0;
    const double alpha = config_.derivativeTau == 0.0 ? 1.0 : dtSeconds / (config_.derivativeTau + dtSeconds);
    state_.filteredDerivative += alpha * (rawDerivative - state_.filteredDerivative);
    const double candidateIntegral = std::clamp(state_.integral + error * dtSeconds, -config_.integralLimit, config_.integralLimit);
    const double unconstrained = config_.kp * error + config_.ki * candidateIntegral + config_.kd * state_.filteredDerivative;
    const double output = std::clamp(unconstrained, config_.outputMin, config_.outputMax);
    const bool saturatedHigh = unconstrained > config_.outputMax && error > 0.0;
    const bool saturatedLow = unconstrained < config_.outputMin && error < 0.0;
    if (!saturatedHigh && !saturatedLow)
        state_.integral = candidateIntegral;
    state_.previousMeasurement = measurement;
    state_.initialized = true;
    return output;
}

void PidController::reset() {
    state_ = {0.0, 0.0, 0.0, false};
}

const PidState& PidController::state() const {
    return state_;
}
