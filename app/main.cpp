#include "controller.hpp"

#include <iostream>

int main() {
  PressureLoop loop({0.0, 4095.0, 0.0, 250.0},
                    {1.8, 0.5, 0.1, 0.0, 100.0, 0.08, 40.0}, 120.0, 1000);
  for (std::uint64_t tick = 1; tick <= 20; ++tick) {
    const auto output =
        loop.sample(1600.0 + tick * 20.0, tick * 100, tick * 100, 0.1);
    std::cout << tick << ',' << output.pressure << ',' << output.valvePercent
              << '\n';
  }
}
