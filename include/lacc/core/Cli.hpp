#pragma once

#include "lacc/core/Runner.hpp"

#include <string>
#include <vector>

namespace lacc::core {

struct CliArgs {
  std::string domain;                   // dna|gw
  RunConfig cfg;                        // core runtime config
  std::vector<std::string> passthrough; // domain-specific args (future)
};

// Parse CLI according to docs/* contracts.
CliArgs parse_cli(int argc, char** argv);

} // namespace lacc::core
