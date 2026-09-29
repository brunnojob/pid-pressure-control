#include "controller.hpp"

#include <cassert>
#include <cmath>
#include <stdexcept>

int main() {
    PidController pid({2.0, 1.0, 0.2, 0.0, 100.0, 0.05, 10.0});
    assert(pid.update(50.0, 40.0, 0.1) > 0.0);
    assert(pid.update(50.0, 0.0, 0.1) == 100.0);
    assert(std::abs(pid.state().integral) <= 10.0);

    PressureLoop loop({0.0, 4095.0, 0.0, 250.0}, {1.0, 0.2, 0.0, 0.0, 100.0, 0.0, 20.0}, 100.0, 500);
    auto good = loop.sample(2047.5, 100, 100, 0.1);
    assert(good.status == LoopStatus::Ready);
    assert(std::abs(good.pressure - 125.0) < 0.01);
    auto stale = loop.sample(2047.5, 100, 1000, 0.1);
    assert(stale.status == LoopStatus::StaleSample);
    assert(loop.sample(2047.5, 1000, 1000, 0.1).valvePercent == 0.0);

    bool rejected = false;
    try {
        pid.update(1.0, 1.0, 0.0);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
}
