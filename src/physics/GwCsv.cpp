#include "lacc/physics/GwCsv.hpp"

#include <stdexcept>

namespace lacc::physics {

GwSeries read_gw_csv(const std::string& path) {
  // STUDENT TODO:
  // Read a two-column CSV file describing a gravitational-wave time series.
  //
  // Required format:
  //   - First line is a header.
  //   - Each remaining non-empty line has exactly two comma-separated fields:
  //       time,strain
  //   - Column 1 parses as a floating-point time value.
  //   - Column 2 parses as a floating-point strain value.
  //
  // Example:
  //   time,strain
  //   0.0,0.01
  //   0.1,0.04
  //   0.2,-0.02
  //
  // Required behavior:
  //   - Throw std::runtime_error if the file cannot be opened.
  //   - Throw std::runtime_error if the file is empty or malformed.
  //   - The returned GwSeries must satisfy:
  //       series.t.size() == series.strain.size()
  (void)path;
  throw std::runtime_error("read_gw_csv() not implemented");
}

} // namespace lacc::physics
