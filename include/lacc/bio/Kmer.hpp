#pragma once
#include <string>
#include <vector>

namespace lacc::bio {

struct KmerTop {
  std::string kmer;
  int count;
  
  bool operator < (const KmerTop& other) const {
    if (count != other.count) {
      return count > other.count;
    }
    return kmer < other.kmer;
  }

  bool operator > (const KmerTop& other) const {
    if (count != other.count) {
      return count < other.count;
    }
    return kmer > other.kmer;
  }
};

  

std::vector<KmerTop> count_kmers_topk(
  const std::vector<std::string>& sequences,
  int k,
  int topk
);

} // namespace lacc::bio
