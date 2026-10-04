#include "lacc/core/Runner.hpp"

#include "lacc/core/DomainPlugin.hpp"
#include "lacc/core/Mpi.hpp"

// Reference (teaching) implementations of the required ADTs.
#include "lacc/core/ReferenceAdts.hpp"

#if defined(_OPENMP)
#  include <omp.h>
#endif

#include <iostream>

namespace lacc::core {

bool should_run_distributed(const RunConfig& cfg) {
#if !defined(LACC_HAS_MPI)
  (void)cfg;
  return false;
#else
  if (cfg.distributed == RunConfig::DistributedMode::Off) return false;
  if (cfg.distributed == RunConfig::DistributedMode::Force) return true;
  // Auto: run distributed only if this process appears to be part of a multi-rank world.
  return size() > 1;
#endif
}

int run(DomainPlugin& plugin, const RunConfig& cfg) {
  // Stage 1/Stage 2 note:
  //   We keep unit tests serial and deterministic. Distributed execution is verified
  //   via a dedicated "smoke" run (see docs) rather than full MPI-based unit tests.
  if (should_run_distributed(cfg)) {
    // Placeholder for Stage 2:
    // - Initialize MPI (if not already)
    // - Distribute work items across ranks
    // - Each rank processes its local subset
    // - Gather/Reduce partial results to rank 0
    // - Rank 0 writes outputs
  #if defined(LACC_HAS_MPI) && LACC_HAS_MPI
      // If Mode is Force, bypass environment checks and try to run distributed.
    if (cfg.distributed == RunConfig::DistributedMode::Force) {
        return true;
    }

    // Handle DistributedMode::Auto
    if (cfg.distributed == RunConfig::DistributedMode::Auto) {
        int flag = 0;
        MPI_Initialized(&flag);
        
        if (flag) {
            // MPI is already active; check if we have multiple ranks
            int size = 1;
            MPI_Comm_size(MPI_COMM_WORLD, &size);
            return size > 1;
        } else {
            // MPI is not initialized yet. Check environment variables 
            const char* mp_size_env = std::getenv("OMPI_COMM_WORLD_SIZE"); // OpenMPI
            if (!mp_size_env) mp_size_env = std::getenv("PMI_SIZE");      // MPICH/Intel
            if (!mp_size_env) mp_size_env = std::getenv("SLURM_NTASKS");  // Slurm

            if (mp_size_env) {
                try {
                    return std::stoi(mp_size_env) > 1;
                } catch (...) {
                    // Fallback if env var parsing fails
                    return false;
                }
            }
        }
    }
#else
    // If the binary wasn't compiled with MPI support, we can't run distributed.
    // (Optional: Log a warning if cfg.distributed was set to Force)
    if (cfg.distributed == RunConfig::DistributedMode::Force) {
        std::cerr << "Warning: DistributedMode::Force requested, but binary compiled without MPI.\n";
    }
#endif

    return 0;
  }

  // Serial path (used by all unit tests in Stage 1).
  auto mapper = plugin.create_feature_mapper();
  auto aggregator = plugin.create_result_aggregator();

  auto items = plugin.enumerate_work();
  for (const auto& wi : items) {
    plugin.process_item(wi, *mapper, cfg);
  }

  // Convert the feature mapper state into a mergeable payload, then merge into the
  // aggregator and write outputs.
  auto pr = plugin.export_partial(*mapper);
  plugin.merge_into_aggregator(pr, *aggregator);

  plugin.write_outputs(*aggregator, cfg);
  return 0;
}


}  // namespace lacc::core
