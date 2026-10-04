#pragma once
#include <vector>

namespace lacc::physics {

struct Peak {
  double time;
  double score;
};

std::vector<Peak> simple_matched_filter_peaks(
  const std::vector<double>& t,
  const std::vector<double>& strain,
  int window,
  double threshold,
  int topk
);

} // namespace lacc::physics
