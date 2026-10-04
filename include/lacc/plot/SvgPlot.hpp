#pragma once

#include <string>
#include <vector>

namespace lacc::plot {

// Writes a simple deterministic SVG bar chart.
// Intended for optional, non-contract artifacts to aid debugging/learning.
void write_bar_chart_svg(const std::vector<std::string>& labels,
                         const std::vector<long long>& values,
                         const std::string& output_path,
                         const std::string& title,
                         const std::string& x_label,
                         const std::string& y_label);

}  // namespace lacc::plot
