// MPI Smoke Test
//
// Purpose:
//   Validate that the project can be compiled and *run* under an MPI launcher.
//   This is intentionally not a correctness/physics/biology test.
//
// How to run (example):
//   RUN_MPI_TESTS=1 ./build_and_run.sh
//
// The test is labeled "mpi" in CTest and excluded by default.

#include <iostream>

#if defined(LACC_HAS_MPI) && LACC_HAS_MPI
  #include <mpi.h>
#endif

int main() {
#if defined(LACC_HAS_MPI) && LACC_HAS_MPI
  MPI_Init(nullptr, nullptr);

  int rank = -1;
  int size = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // We expect this test to be launched with 2 ranks.
  if (size != 2) {
    if (rank == 0) {
      std::cerr << "test_mpi_smoke expected size=2 but got size=" << size << "\n";
    }
    MPI_Finalize();
    return 2;
  }

  int sum = 0;
  MPI_Reduce(&rank, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    // ranks {0,1} => sum == 1
    if (sum != 1) {
      std::cerr << "MPI_Reduce produced unexpected sum=" << sum << "\n";
      MPI_Finalize();
      return 3;
    }
    std::cout << "MPI smoke test passed (size=2)\n";
  }

  MPI_Finalize();
  return 0;
#else
  // Built without MPI support; nothing to validate at runtime.
  std::cout << "MPI smoke test skipped (built without MPI)\n";
  return 0;
#endif
}
