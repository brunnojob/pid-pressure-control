#include <cstdlib>
#include "identification.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "usage: identify-process samples.csv dt_seconds\n";
    return 2;
  }
  try {
    char *end;
    double dt = std::strtod(argv[2], &end);
    if (*end)
      throw std::invalid_argument("invalid sample interval");
    std::ifstream file(argv[1]);
    if (!file)
      throw std::runtime_error("cannot open samples");
    std::vector<ProcessSample> samples;
    std::string line;
    while (std::getline(file, line)) {
      ProcessSample sample;
      char separator, extra;
      std::istringstream row(line);
      if (!(row >> sample.input >> separator >> sample.output) ||
          separator != ',' || row >> extra)
        throw std::invalid_argument("invalid CSV row");
      if (samples.size() == 1000000)
        throw std::invalid_argument("sample limit exceeded");
      samples.push_back(sample);
    }
    auto model = identify(samples, dt);
    std::cout << "{\"pole\":" << model.pole << ",\"gain\":" << model.gain
              << ",\"offset\":" << model.offset
              << ",\"timeConstant\":" << model.timeConstant
              << ",\"residualRms\":" << model.residualRms << "}\n";
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
    return 2;
  }
}
