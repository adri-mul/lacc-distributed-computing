#include "lacc/physics/MatchedFilter.hpp"
#include "lacc/adt/PriorityQueue.hpp"

#include <stdexcept>

namespace lacc::physics {

std::vector<Peak> simple_matched_filter_peaks(
  const std::vector<double>& t,
  const std::vector<double>& strain,
  int window,
  double threshold,
  int topk
) {
  // STUDENT TODO:
  // Analyze the input time series and identify the strongest candidate signal
  // artifacts present in the data.
  //
  // Required behavior:
  //   - Use the time and strain vectors together as one series.
  //   - Build a score for each valid position using a sliding window of width
  //     'window'.
  //   - Keep local maxima whose score is >= threshold.
  //   - Retain the strongest candidate peaks using the course PriorityQueue ADT.
  //   - Return peaks in deterministic final order for the same input.
  //
  // Important:
  //   The reported peaks depend on the contents of the input signal.
  //   Different valid inputs will produce different valid outputs.
  //
  // Recommended tie-break rule for final output:
  //   - primary key: descending score
  //   - secondary key: ascending time
  (void)t;
  (void)strain;
  (void)window;
  (void)threshold;
  (void)topk;
  throw std::runtime_error("simple_matched_filter_peaks() not implemented");
}

} // namespace lacc::physics
