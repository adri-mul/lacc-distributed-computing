#pragma once

#include <cstdint>
#include <string>

namespace lacc::core {

// A unit of work distributed across MPI ranks.
// The domain plugin decides how to interpret these fields.
struct WorkItem {
  std::uint32_t id = 0;
  std::uint64_t offset = 0;
  std::uint64_t length = 0;
  std::string source; // e.g., input filename, shard id, etc.
};

} // namespace lacc::core
