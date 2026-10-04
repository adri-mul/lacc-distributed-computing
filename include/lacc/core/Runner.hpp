#pragma once

#include <cstddef>
#include <string>

namespace lacc::core {

class DomainPlugin;

struct RunConfig {
  enum class DistributedMode {
    Off,   // never attempt distributed execution
    Auto,  // distributed only when launched with >1 rank
    Force  // attempt distributed even if heuristics say "no"
  };

  // Contract-ish inputs
  std::string output_dir = ".";
  std::size_t topk = 5;

  // Parallel controls (Stage 2 will make these do real work)
  int threads = 1;
  DistributedMode distributed = DistributedMode::Off;
};

// Stage 1 note: tests remain serial; distributed execution is "opt-in" and
// only becomes meaningful once stage-2 MPI work distribution is implemented.
bool should_run_distributed(const RunConfig& cfg);

// Execute the pipeline for a selected domain plugin.
int run(DomainPlugin& plugin, const RunConfig& cfg);

}  // namespace lacc::core
