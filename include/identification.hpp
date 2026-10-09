#pragma once
#include <array>
#include <cmath>
#include <stdexcept>
#include <vector>
struct ProcessSample {
  double input, output;
};
struct ProcessModel {
  double pole, gain, offset, timeConstant, residualRms;
};
inline ProcessModel identify(const std::vector<ProcessSample> &samples,
                             double dt) {
  if (samples.size() < 10 || !std::isfinite(dt) || dt <= 0)
    throw std::invalid_argument(
        "at least ten regularly sampled points required");
  std::array<std::array<double, 4>, 3> equations{};
  for (std::size_t i = 0; i + 1 < samples.size(); i++) {
    if (!std::isfinite(samples[i].input) || !std::isfinite(samples[i].output) ||
        !std::isfinite(samples[i + 1].output))
      throw std::invalid_argument("non-finite process sample");
    std::array<double, 3> x{samples[i].output, samples[i].input, 1};
    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++)
        equations[r][c] += x[r] * x[c];
      equations[r][3] += x[r] * samples[i + 1].output;
    }
  }
  for (int column = 0; column < 3; column++) {
    int pivot = column;
    for (int r = column + 1; r < 3; r++)
      if (std::abs(equations[r][column]) > std::abs(equations[pivot][column]))
        pivot = r;
    std::swap(equations[column], equations[pivot]);
    double divisor = equations[column][column];
    if (std::abs(divisor) < 1e-9)
      throw std::invalid_argument("insufficient input excitation");
    for (int c = column; c < 4; c++)
      equations[column][c] /= divisor;
    for (int r = 0; r < 3; r++)
      if (r != column) {
        double scale = equations[r][column];
        for (int c = column; c < 4; c++)
          equations[r][c] -= scale * equations[column][c];
      }
  }
  double a = equations[0][3], b = equations[1][3], c = equations[2][3];
  if (a <= 0 || a >= 1)
    throw std::invalid_argument("process is not a stable first-order model");
  double square = 0;
  for (std::size_t i = 0; i + 1 < samples.size(); i++) {
    double residual = samples[i + 1].output -
                      (a * samples[i].output + b * samples[i].input + c);
    square += residual * residual;
  }
  return {a, b / (1 - a), c / (1 - a), -dt / std::log(a),
          std::sqrt(square / (samples.size() - 1))};
}
