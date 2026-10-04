#pragma once

#include <string>
#include <vector>

namespace lacc::core {

// A minimal representation of a per-rank or per-task partial output.
//
// Stage 1 (serial correctness) intent:
//   - This exists mostly so the "core" runner can compile cleanly while
//     we introduce MPI/OpenMP concepts and leave TODO hooks.
//   - The reference demos and tests remain serial.
//
// Stage 2 (distributed correctness) intent:
//   - We can serialize this container (keys + values) for gather/reduce.
//   - We can also swap to a denser binary format later without changing
//     the domain plugins' high-level shape.
struct PartialResult {
  std::vector<std::string> keys;
  std::vector<double> values;
};

}  // namespace lacc::core
