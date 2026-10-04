#pragma once

// This header provides a thin wrapper around MPI calls.
//
// Design goal:
//   - The project must compile and run in environments without MPI installed.
//   - When MPI is available, we can build a "real" MPI binary.
//
// See docs/PARALLEL_TUTORIAL.md for an explanation of the MPI lifecycle.

#ifdef LACC_HAS_MPI
  #include <mpi.h>
#endif

namespace lacc::core {

class Mpi {
 public:
  static constexpr bool available() {
#ifdef LACC_HAS_MPI
    return true;
#else
    return false;
#endif
  }

  Mpi(int* argc, char*** argv) {
#ifdef LACC_HAS_MPI
    // [MPI-SETUP-1] Initialize MPI (must happen before most MPI calls).
    MPI_Init(argc, argv);
#endif
  }

  ~Mpi() {
#ifdef LACC_HAS_MPI
    // [MPI-SETUP-2] Finalize MPI (all ranks must reach this point).
    MPI_Finalize();
#endif
  }

  int rank() const {
#ifdef LACC_HAS_MPI
    int r = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &r);
    return r;
#else
    return 0;
#endif
  }

  int size() const {
#ifdef LACC_HAS_MPI
    int n = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    return n;
#else
    return 1;
#endif
  }

  void barrier() const {
#ifdef LACC_HAS_MPI
    // [MPI-SYNC-1] Barrier synchronization.
    MPI_Barrier(MPI_COMM_WORLD);
#endif
  }
};

// Convenience helpers used throughout the teaching code.
//
// These are intentionally simple:
//   - If built without MPI (default stage-1), they return rank=0, size=1.
//   - If built with MPI, they query MPI_COMM_WORLD.
//
// Note: Stage-1 unit tests are serial; even if MPI is available on the system,
// tests should run without `mpirun` so these return (0,1) in normal runs.
inline int rank() {
#ifdef LACC_HAS_MPI
  int flag = 0;
  MPI_Initialized(&flag);
  if (!flag) return 0;
  int r = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &r);
  return r;
#else
  return 0;
#endif
}

inline int size() {
#ifdef LACC_HAS_MPI
  int flag = 0;
  MPI_Initialized(&flag);
  if (!flag) return 1;
  int s = 1;
  MPI_Comm_size(MPI_COMM_WORLD, &s);
  return s;
#else
  return 1;
#endif
}

// Environment-only size detection (works even when MPI is not compiled in).
// Used by should_run_distributed() to avoid changing unit tests.
int hinted_world_size_from_env();

}  // namespace lacc::core
