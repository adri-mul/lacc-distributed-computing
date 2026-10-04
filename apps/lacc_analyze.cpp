#include "../include/lacc/bio/Fasta.hpp"
#include "../include/lacc/bio/Kmer.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

// Minimal, deterministic SVG bar chart writer.
// - No external deps
// - Stable output across environments
// - Intended as *optional* visibility only (never part of contract)
void write_bar_svg(const std::string& svg_path,
                   const std::string& title,
                   const std::string& xlabel,
                   const std::string& ylabel,
                   const std::vector<std::string>& labels,
                   const std::vector<double>& values) {
  if (labels.size() != values.size()) {
    throw std::runtime_error("write_bar_svg: labels/values size mismatch");
  }

  const int W = 900;
  const int H = 420;

  const int margin_left = 70;
  const int margin_right = 20;
  const int margin_top = 50;
  const int margin_bottom = 80;

  const int plot_w = W - margin_left - margin_right;
  const int plot_h = H - margin_top - margin_bottom;

  double vmax = 0.0;
  for (double v : values) vmax = std::max(vmax, v);
  if (vmax <= 0.0) vmax = 1.0;  // avoid div by zero

  std::ofstream out(svg_path);
  if (!out) throw std::runtime_error("Failed to open SVG for writing: " + svg_path);

  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << W << "\" height=\"" << H
      << "\" viewBox=\"0 0 " << W << " " << H << "\">\n";

  // Background
  out << "<rect x=\"0\" y=\"0\" width=\"" << W << "\" height=\"" << H
      << "\" fill=\"white\" />\n";

  // Title
  out << "<text x=\"" << (W / 2) << "\" y=\"28\" text-anchor=\"middle\" "
      << "font-family=\"sans-serif\" font-size=\"18\" fill=\"black\">"
      << title << "</text>\n";

  // Axes
  const int x0 = margin_left;
  const int y0 = margin_top + plot_h;
  const int x1 = margin_left + plot_w;
  const int y1 = margin_top;

  out << "<line x1=\"" << x0 << "\" y1=\"" << y0 << "\" x2=\"" << x1 << "\" y2=\"" << y0
      << "\" stroke=\"black\" stroke-width=\"2\" />\n";
  out << "<line x1=\"" << x0 << "\" y1=\"" << y0 << "\" x2=\"" << x0 << "\" y2=\"" << y1
      << "\" stroke=\"black\" stroke-width=\"2\" />\n";

  // Y ticks (0..vmax) with 5 ticks
  const int ticks = 5;
  for (int i = 0; i <= ticks; ++i) {
    const double frac = static_cast<double>(i) / ticks;
    const int y = y0 - static_cast<int>(std::round(frac * plot_h));
    const double val = frac * vmax;

    out << "<line x1=\"" << (x0 - 5) << "\" y1=\"" << y
        << "\" x2=\"" << x0 << "\" y2=\"" << y
        << "\" stroke=\"black\" stroke-width=\"1\" />\n";

    out << "<text x=\"" << (x0 - 10) << "\" y=\"" << (y + 4)
        << "\" text-anchor=\"end\" font-family=\"sans-serif\" font-size=\"12\" fill=\"black\">"
        << val << "</text>\n";
  }

  // Axis labels
  out << "<text x=\"" << (W / 2) << "\" y=\"" << (H - 16)
      << "\" text-anchor=\"middle\" font-family=\"sans-serif\" font-size=\"14\" fill=\"black\">"
      << xlabel << "</text>\n";

  // y-label rotated
  out << "<text x=\"18\" y=\"" << (H / 2)
      << "\" transform=\"rotate(-90 18 " << (H / 2)
      << ")\" text-anchor=\"middle\" font-family=\"sans-serif\" font-size=\"14\" fill=\"black\">"
      << ylabel << "</text>\n";

  // Bars
  const int n = static_cast<int>(labels.size());
  if (n > 0) {
    const double slot = static_cast<double>(plot_w) / n;
    const double bar_w = slot * 0.7;
    const double bar_pad = slot * 0.15;

    for (int i = 0; i < n; ++i) {
      const double v = values[i];
      const double h = (v / vmax) * plot_h;

      const double x = x0 + i * slot + bar_pad;
      const double y = y0 - h;

      out << "<rect x=\"" << x << "\" y=\"" << y
          << "\" width=\"" << bar_w << "\" height=\"" << h
          << "\" fill=\"#4e79a7\" />\n";

      // value label above bar
      out << "<text x=\"" << (x + bar_w / 2) << "\" y=\"" << (y - 6)
          << "\" text-anchor=\"middle\" font-family=\"sans-serif\" font-size=\"12\" fill=\"black\">"
          << v << "</text>\n";

      // x tick label (rotated for readability)
      const double tx = x + bar_w / 2;
      const double ty = y0 + 18;

      out << "<text x=\"" << tx << "\" y=\"" << ty
          << "\" transform=\"rotate(35 " << tx << " " << ty
          << ")\" text-anchor=\"start\" font-family=\"monospace\" font-size=\"12\" fill=\"black\">"
          << labels[i] << "</text>\n";
    }
  }

  // Footer note (helps students trust it)
  out << "<text x=\"" << (W - 10) << "\" y=\"" << (H - 10)
      << "\" text-anchor=\"end\" font-family=\"sans-serif\" font-size=\"11\" fill=\"#555\">"
      << "Optional artifact: visualization only (does not affect contract outputs)"
      << "</text>\n";

  out << "</svg>\n";
}

bool parse_plots_flag(int argc, char** argv) {
  // We keep the first three args positional and allow optional flags after.
  // Unknown flags are rejected to avoid silent mistakes.
  for (int i = 4; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--plots") continue;
    if (a == "-h" || a == "--help") continue;
    throw std::runtime_error("Unknown option: " + a);
  }
  for (int i = 4; i < argc; ++i) {
    if (std::string(argv[i]) == "--plots") return true;
  }
  return false;
}

void print_usage() {
  std::cerr
      << "Usage:\n"
      << "  lacc_analyze <gw|dna> <input_file> <output_prefix> [--plots]\n\n"
      << "Notes:\n"
      << "  - Contract outputs are the CSV files.\n"
      << "  - --plots generates additional SVG artifacts only.\n";
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 5) {
    print_usage();
    return 1;
  }

  std::string input_file;
  std::string track;
  std::string k;
  std::string topK;
  std::string threshold;
  std::string out_prefix;

  for (int i = 1; i < argc; ++i) {
    const std::string a = argv[i];
    if (a == "--input") {
      input_file = argv[i+1];
    } if (a == "--mode") {
      track = argv[i+1];
    } if (a == "--k") {
      k = argv[i+1];
    } if (a == "--topk" || a == "--topK") {
      topK = argv[i+1];
    } if (a == "--threshold" || a == "--t") {
      threshold = argv[i+1];
    } if (a == "--outdir") {
      out_prefix = argv[i+1];
    }
  }

  bool plots = true;
  try {
    // STUDENT NOTE:
    // This command path depends on student implementations of:
    //   - input parsing
    //   - artifact extraction logic
    //   - deterministic output behavior
    if (argc >= 12) {
      // allow --help as an optional extra arg
      for (int i = 7; i < argc; ++i) {
        const std::string a = argv[i];
        if (a == "-h" || a == "--help") {
          print_usage();
          return 0;
        }
      }
      plots = parse_plots_flag(argc, argv);
    }
  } catch (const std::exception& e) {
    std::cout << "yes papi" << std::endl;
    std::cerr << "ERROR: " << e.what() << "\n\n";
    print_usage();
    return 1;
  }

  try {
    // STUDENT NOTE:
    // This command path depends on student implementations of:
    //   - input parsing
    //   - artifact extraction logic
    //   - deterministic output behavior
    /* if (track == "gw") {
      auto series = lacc::physics::read_gw_csv(input_file);
      auto peaks =
          lacc::physics::simple_matched_filter_peaks(series.t, series.strain, 4, 0.25, 5);

      const std::string csv_path = out_prefix + "_peaks.csv";
      std::ofstream out(csv_path);
      out << "time,score\n";
      for (const auto& p : peaks) out << p.time << "," << p.score << "\n";
      std::cout << "Wrote " << csv_path << "\n";

      if (plots) {
        std::vector<std::string> labels;
        std::vector<double> values;
        labels.reserve(peaks.size());
        values.reserve(peaks.size());
        for (const auto& p : peaks) {
          labels.push_back(std::to_string(p.time));
          values.push_back(p.score);
        }
        const std::string svg_path = out_prefix + "_peaks.svg";
        write_bar_svg(svg_path, "GW Peaks (matched filter)", "time", "score", labels, values);
        std::cout << "Wrote " << svg_path << "\n";
      }

    }  */if (track == "dna") {
      auto reads = lacc::bio::read_fasta_like(input_file);
      std::vector<std::string> seqs;
      seqs.reserve(reads.size());
      for (const auto& r : reads) seqs.push_back(r.seq);

      auto tops = lacc::bio::count_kmers_topk(seqs, std::stoi(k), std::stoi(topK));

      const std::string csv_path = out_prefix + "_kmers.csv";
      std::ofstream out(csv_path);
      out << "kmer,count\n";
      for (const auto& k : tops) out << k.kmer << "," << k.count << "\n";
      std::cout << "Wrote " << csv_path << "\n";

      if (plots) {
        std::vector<std::string> labels;
        std::vector<double> values;
        labels.reserve(tops.size());
        values.reserve(tops.size());
        for (const auto& k : tops) {
          labels.push_back(k.kmer);
          values.push_back(static_cast<double>(k.count));
        }
        const std::string svg_path = out_prefix + "_kmers.svg";
        write_bar_svg(svg_path, "DNA Top-K k-mers", "kmer", "count", labels, values);
        std::cout << "Wrote " << svg_path << "\n";
      }

    } else {
      std::cerr << "Unknown track: " << track << "\n";
      print_usage();
      return 2;
    }
  } catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << "\n";
    return 3;
  }

  return 0;
}

