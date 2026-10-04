#pragma once
#include <string>
#include <vector>

namespace lacc::bio {

struct Read {
  std::string id;
  std::string seq;
};

std::vector<Read> read_fasta_like(const std::string& path);

} // namespace lacc::bio
