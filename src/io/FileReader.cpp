#include "lacc/io/FileReader.hpp"

#include <stdexcept>
#include <fstream>

namespace lacc::io {

std::vector<std::string> read_all_lines(const std::string& path) {
  // STUDENT TODO:
  // Open a plain-text file and return all lines in order.
  //
  // Required behavior:
  //   - Throw std::runtime_error if the file cannot be opened.
  //   - Return lines exactly as read, in order.
  //   - Preserve empty lines. Higher-level parsers may need them.
  //   - This helper may be reused by FASTA-like and CSV parsers.
  std::ifstream file (path);
  if (!file.is_open()) {
    throw std::runtime_error("file cannot be opened.");
  }
  std::vector<std::string> lines;
  std::string line;
  while (getline(file, line)) {
    lines.push_back(line);
  }

  return lines;
}

} // namespace lacc::io
