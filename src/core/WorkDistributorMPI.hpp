#pragma once
#include "lacc/core/Work.hpp"
#include <vector>

namespace lacc::core {

// Dynamic master–worker distribution.
// When MPI is unavailable, worker_get_next will return false immediately.

void master_distribute(const std::vector<WorkItem>& items);
bool worker_get_next(WorkItem& out);

} // namespace lacc::core
