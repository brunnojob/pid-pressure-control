#include "identification.hpp"
#include <cassert>
int main() {
  std::vector<ProcessSample> samples;
  double y = 0;
  for (int i = 0; i < 100; i++) {
    double u = i % 17 < 8 ? 5 : 1;
    samples.push_back({u, y});
    y = 0.8 * y + 0.4 * u + 0.1;
  }
  auto model = identify(samples, 1);
  assert(std::abs(model.pole - 0.8) < 1e-8);
  assert(std::abs(model.gain - 2) < 1e-8);
  assert(model.residualRms < 1e-8);
  bool rejected = false;
  try {
    identify(std::vector<ProcessSample>(20, {1, 1}), 1);
  } catch (const std::invalid_argument &) {
    rejected = true;
  }
  assert(rejected);
}
