#pragma once
#include <string>
#include <vector>

namespace lacc::physics {

struct GwSeries {
  std::vector<double> t;
  std::vector<double> strain;
};

GwSeries read_gw_csv(const std::string& path);

} // namespace lacc::physics
