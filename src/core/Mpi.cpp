#include "lacc/core/Mpi.hpp"

#include <cstdlib>

namespace lacc::core {

static int read_env_int(const char* name) {
  const char* v = std::getenv(name);
  if (!v || !*v) return -1;
  char* end = nullptr;
  long n = std::strtol(v, &end, 10);
  if (end == v) return -1;
  if (n < 0) return -1;
  if (n > 1'000'000) return -1;
  return static_cast<int>(n);
}

int hinted_world_size_from_env() {
  // Common environment variables set by different MPI launchers / schedulers.
  // We check a handful in order of "most likely".
  const int ompi = read_env_int("OMPI_COMM_WORLD_SIZE");
  if (ompi > 0) return ompi;

  const int pmi = read_env_int("PMI_SIZE");
  if (pmi > 0) return pmi;

  const int slurm_ntasks = read_env_int("SLURM_NTASKS");
  if (slurm_ntasks > 0) return slurm_ntasks;

  const int mvapich2 = read_env_int("MV2_COMM_WORLD_SIZE");
  if (mvapich2 > 0) return mvapich2;

  return 1;
}

}  // namespace lacc::core
