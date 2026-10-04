#include "lacc/core/Cli.hpp"
#include "lacc/core/Mpi.hpp"
#include "lacc/core/Runner.hpp"
#include "lacc/core/DomainPlugin.hpp"

#include "lacc/core/ReferenceAdts.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

#if defined(LACC_HAS_MPI) && LACC_HAS_MPI
  #include <mpi.h>
#endif

namespace fs = std::filesystem;

// Stub DNA and GW plugins so the core framework compiles and runs.
// These will be replaced by real domain plugins in the next phase.
namespace {

class DnaStubPlugin final : public lacc::core::DomainPlugin {
public:
  std::string name() const override { return "dna-stub"; }

  std::vector<lacc::core::WorkItem> enumerate_work() override {
    // Starter: no real work yet. This keeps lacc_exec deterministic and safe.
    return {};
  }

  std::unique_ptr<lacc::core::IFeatureMapper> create_feature_mapper() override {
    return std::make_unique<lacc::core::ReferenceFeatureMapper>();
  }

  std::unique_ptr<lacc::core::IResultAggregator> create_result_aggregator() override {
    return std::make_unique<lacc::core::ReferenceResultAggregator>();
  }

  void process_item(const lacc::core::WorkItem&, lacc::core::IFeatureMapper&, const lacc::core::RunConfig&) override {
    // no-op
  }

  lacc::core::PartialResult export_partial(const lacc::core::IFeatureMapper& mapper) override {
    lacc::core::PartialResult pr;
    auto* ref = dynamic_cast<const lacc::core::ReferenceFeatureMapper*>(&mapper);
    if (!ref) return pr;
    pr.keys.reserve(ref->data().size());
    pr.values.reserve(ref->data().size());
    for (const auto& [k, v] : ref->data()) {
      pr.keys.push_back(k);
      pr.values.push_back(v);
    }
    return pr;
  }

  void merge_into_aggregator(const lacc::core::PartialResult& pr, lacc::core::IResultAggregator& agg) override {
    // In real DNA, keys map to taxonomy paths. Stub: treat key as leaf path.
    for (std::size_t i = 0; i < pr.keys.size(); ++i) {
      agg.add_leaf_value(pr.keys[i], pr.values[i]);
    }
  }

  void write_outputs(const lacc::core::IResultAggregator& agg, const lacc::core::RunConfig& cfg) override {
    fs::create_directories(cfg.output_dir);

    // Minimal contract-shaped outputs (empty for now).
    {
      std::ofstream f(fs::path(cfg.output_dir) / "dna_results.csv");
      f << "taxon_path,taxon_name,value,level,is_leaf\n";
      for (const auto& r : agg.to_rows_preorder()) {
        f << r.taxon_path << "," << r.taxon_name << "," << r.value << "," << r.level << "," << (r.is_leaf ? "true" : "false") << "\n";
      }
    }
    {
      std::ofstream f(fs::path(cfg.output_dir) / "dna_topk.csv");
      f << "rank,taxon_path,taxon_name,value\n";
      // Empty in stub.
    }
  }
};

class GwStubPlugin final : public lacc::core::DomainPlugin {
public:
  std::string name() const override { return "gw-stub"; }
  std::vector<lacc::core::WorkItem> enumerate_work() override { return {}; }
  std::unique_ptr<lacc::core::IFeatureMapper> create_feature_mapper() override {
    return std::make_unique<lacc::core::ReferenceFeatureMapper>();
  }
  std::unique_ptr<lacc::core::IResultAggregator> create_result_aggregator() override {
    return std::make_unique<lacc::core::ReferenceResultAggregator>();
  }
  void process_item(const lacc::core::WorkItem&, lacc::core::IFeatureMapper&, const lacc::core::RunConfig&) override {}
  lacc::core::PartialResult export_partial(const lacc::core::IFeatureMapper&) override { return {}; }
  void merge_into_aggregator(const lacc::core::PartialResult&, lacc::core::IResultAggregator&) override {}
  void write_outputs(const lacc::core::IResultAggregator& agg, const lacc::core::RunConfig& cfg) override {
    fs::create_directories(cfg.output_dir);
    std::ofstream f(fs::path(cfg.output_dir) / "gw_results.csv");
    f << "taxon_path,taxon_name,value,level,is_leaf\n";
    for (const auto& r : agg.to_rows_preorder()) {
      f << r.taxon_path << "," << r.taxon_name << "," << r.value << "," << r.level << "," << (r.is_leaf ? "true" : "false") << "\n";
    }
  }
};

std::unique_ptr<lacc::core::DomainPlugin> make_plugin(const std::string& domain) {
  if (domain == "dna") return std::make_unique<DnaStubPlugin>();
  if (domain == "gw")  return std::make_unique<GwStubPlugin>();
  throw std::runtime_error("Unknown --domain: " + domain + " (expected 'dna' or 'gw')");
}

} // namespace

int main(int argc, char** argv) {
#if defined(LACC_HAS_MPI) && LACC_HAS_MPI
  MPI_Init(&argc, &argv);
#endif

  try {
    auto cli = lacc::core::parse_cli(argc, argv);
    auto plugin = make_plugin(cli.domain);

    if (lacc::core::rank() == 0) {
      std::cerr << "==> lacc_exec starting (domain=" << cli.domain << ", mpi_ranks=" << lacc::core::size() << ")\n";
    }

    int rc = lacc::core::run(*plugin, cli.cfg);

#if defined(LACC_HAS_MPI) && LACC_HAS_MPI
    MPI_Finalize();
#endif
    return rc;
  } catch (const std::exception& e) {
    if (lacc::core::rank() == 0) std::cerr << "Error: " << e.what() << "\n";
#if defined(LACC_HAS_MPI) && LACC_HAS_MPI
    MPI_Finalize();
#endif
    return 2;
  }
}
