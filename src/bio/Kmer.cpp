#include "lacc/bio/Kmer.hpp"
#include "lacc/adt/HashTableChained.hpp"
#include "lacc/adt/PriorityQueue.hpp"
#include <iostream>

#include <stdexcept>

namespace lacc::bio {

std::vector<KmerTop> count_kmers_topk(
  const std::vector<std::string>& sequences,
  int k,
  int topk
) {
  // STUDENT TODO:
  // Analyze the input sequences and identify the most significant k-mer
  // artifacts present in the source data.
  //
  // Required behavior:
  //   - Consider every overlapping substring of length k.
  //   - Ignore sequences shorter than k.
  //   - Count occurrences using the course HashTableChained ADT.
  //   - Retain the strongest artifact candidates using the course
  //     PriorityQueue ADT.
  //   - Return results in deterministic order for the same input.
  //
  // Important:
  //   The reported k-mers depend on the contents of the input file.
  //   Different valid inputs will produce different valid outputs.
  //
  // Recommended tie-break rule for final output:
  //   - primary key: descending count
  //   - secondary key: ascending k-mer string
  //
  // Parallel note:
  //   Serial correctness comes first. The provided framework is allowed to run
  //   with MPI/OpenMP later, but your initial implementation should be correct
  //   and deterministic in the serial case.

  // sliding char window
  std::string str_window;
  lacc::adt::HashTableChained<int> *kmerCount = new lacc::adt::HashTableChained<int>();
  lacc::adt::PriorityQueue<KmerTop, std::greater<KmerTop>> *pq = new lacc::adt::PriorityQueue<KmerTop, std::greater<KmerTop>>();
  KmerTop kmer;
  for (const std::string& sequence : sequences) {
    if (sequence.length() < static_cast<size_t>(k)) continue; // strings have a '\0' null terminating character, so "happy" will be 'h' 'a' 'p' 'p' 'y' '\0' (len 6)
    for (size_t i = 0; i <= sequence.length() - k; ++i) {
      str_window = sequence.substr(i, k);

      // logic to update kmer counts
      if (kmerCount->contains(str_window)) { // key is str_window, value is count
        kmerCount->increment(str_window, 1);
      } else {
        kmerCount->put(str_window, 1);
      }
    }
  }

  // Take the top topk highest count kmers and put it into a priority queue
  // pair of kmer, count of kmer
  std::vector<std::pair<std::string, int>> buckets = kmerCount->entries();

  // check to make sure bucket has some data
  if (buckets.empty()) {
    return {};
  }

  while (pq->size() < static_cast<size_t>(topk) && !buckets.empty()) {
    // find highest value
    std::pair<std::string, int> highest = buckets[0];
    size_t highest_pos = 0;
    for (size_t i = 1; i < buckets.size(); ++i) {
      if (buckets[i].second > highest.second || (buckets[i].second == highest.second && buckets[i].first < highest.first)) {
        highest = buckets[i];
        highest_pos = i;
      }
    }
    // move into priority queue
    kmer.kmer = highest.first;
    kmer.count = highest.second;
    pq->push(*&kmer);
    std::swap(buckets[highest_pos], buckets[buckets.size()-1]);
    buckets.pop_back();
  }
  
  std::vector<KmerTop> output;
  while (!pq->empty()) {
    output.push_back(pq->top());
    pq->pop();
  }

  return output;
}

} // namespace lacc::bio
