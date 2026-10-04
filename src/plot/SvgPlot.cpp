#include "lacc/plot/SvgPlot.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace lacc::plot {

static std::string xml_escape(const std::string& s) {
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    switch (c) {
      case '&': out += "&amp;"; break;
      case '<': out += "&lt;"; break;
      case '>': out += "&gt;"; break;
      case '"': out += "&quot;"; break;
      case '\'': out += "&apos;"; break;
      default: out += c; break;
    }
  }
  return out;
}

void write_bar_chart_svg(const std::vector<std::string>& labels,
                         const std::vector<long long>& values,
                         const std::string& output_path,
                         const std::string& title,
                         const std::string& x_label,
                         const std::string& y_label) {
  if (labels.size() != values.size()) {
    throw std::runtime_error("write_bar_chart_svg: labels/values size mismatch");
  }

  const int width = 900;
  const int height = 500;
  const int margin_left = 70;
  const int margin_right = 20;
  const int margin_top = 40;
  const int margin_bottom = 90;

  const int plot_w = width - margin_left - margin_right;
  const int plot_h = height - margin_top - margin_bottom;

  long long vmax = 0;
  for (auto v : values) vmax = std::max(vmax, v);
  if (vmax <= 0) vmax = 1;

  std::ofstream out(output_path);
  if (!out) throw std::runtime_error("Failed to open plot output: " + output_path);

  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << width
      << "\" height=\"" << height << "\" viewBox=\"0 0 " << width << " "
      << height << "\">\n";
  out << "<rect x=\"0\" y=\"0\" width=\"" << width
      << "\" height=\"" << height << "\" fill=\"white\"/>\n";

  // Title
  out << "<text x=\"" << width / 2 << "\" y=\"24\" text-anchor=\"middle\" "
         "font-family=\"sans-serif\" font-size=\"18\">"
      << xml_escape(title) << "</text>\n";

  const int x0 = margin_left;
  const int y0 = margin_top + plot_h;

  // Axes
  out << "<line x1=\"" << x0 << "\" y1=\"" << margin_top
      << "\" x2=\"" << x0 << "\" y2=\"" << y0
      << "\" stroke=\"black\"/>\n";
  out << "<line x1=\"" << x0 << "\" y1=\"" << y0
      << "\" x2=\"" << (x0 + plot_w) << "\" y2=\"" << y0
      << "\" stroke=\"black\"/>\n";

  // Y ticks (5)
  for (int i = 0; i <= 5; ++i) {
    const double frac = static_cast<double>(i) / 5.0;
    const int y = y0 - static_cast<int>(std::round(frac * plot_h));
    out << "<line x1=\"" << (x0 - 5) << "\" y1=\"" << y
        << "\" x2=\"" << x0 << "\" y2=\"" << y
        << "\" stroke=\"black\"/>\n";
    const long long val = static_cast<long long>(std::llround(frac * vmax));
    out << "<text x=\"" << (x0 - 10) << "\" y=\"" << (y + 4)
        << "\" text-anchor=\"end\" font-family=\"sans-serif\" font-size=\"12\">"
        << val << "</text>\n";
  }

  const size_t n = labels.size();
  if (n == 0) {
    out << "</svg>\n";
    return;
  }
  const double step = static_cast<double>(plot_w) / static_cast<double>(n);
  const double bar_w = step * 0.72;

  for (size_t i = 0; i < n; ++i) {
    const double x = x0 + i * step + (step - bar_w) / 2.0;
    const double h = (static_cast<double>(values[i]) / static_cast<double>(vmax)) * plot_h;
    const double y = y0 - h;

    out << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << bar_w
        << "\" height=\"" << h << "\" fill=\"steelblue\"/>\n";

    // X label (rotated)
    const double lx = x + bar_w / 2.0;
    const double ly = y0 + 18;
    out << "<text x=\"" << lx << "\" y=\"" << ly
        << "\" text-anchor=\"end\" font-family=\"sans-serif\" font-size=\"12\""
           " transform=\"rotate(-45 "
        << lx << " " << ly << ")\">"
        << xml_escape(labels[i]) << "</text>\n";
  }

  // Axis labels
  out << "<text x=\"" << (x0 + plot_w / 2) << "\" y=\"" << (height - 20)
      << "\" text-anchor=\"middle\" font-family=\"sans-serif\" font-size=\"14\">"
      << xml_escape(x_label) << "</text>\n";

  // Y label (rotated)
  out << "<text x=\"20\" y=\"" << (margin_top + plot_h / 2)
      << "\" text-anchor=\"middle\" font-family=\"sans-serif\" font-size=\"14\""
         " transform=\"rotate(-90 20 "
      << (margin_top + plot_h / 2) << ")\">"
      << xml_escape(y_label) << "</text>\n";

  out << "</svg>\n";
}

}  // namespace lacc::plot
