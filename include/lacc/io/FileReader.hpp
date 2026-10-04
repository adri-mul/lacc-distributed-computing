#pragma once
#include <string>
#include <vector>

namespace lacc::io {
std::vector<std::string> read_all_lines(const std::string& path);
} // namespace lacc::io
