#pragma once

#include "lacc/core/Runner.hpp"
#include "lacc/core/PartialResult.hpp"
#include "lacc/core/Work.hpp"

#include <memory>
#include <string>
#include <vector>

namespace lacc::core {

class IFeatureMapper;
class IResultAggregator;

// DomainPlugin defines domain-specific behavior while the core framework owns:
//  - MPI distribution protocol
//  - deterministic merge/aggregation pipeline
class DomainPlugin {
public:
  virtual ~DomainPlugin() = default;

  virtual std::string name() const = 0;

  // Enumerate work items for this run (root rank typically enumerates; framework distributes).
  virtual std::vector<WorkItem> enumerate_work() = 0;

  // Factory methods to create per-rank ADTs.
  virtual std::unique_ptr<IFeatureMapper> create_feature_mapper() = 0;
  virtual std::unique_ptr<IResultAggregator> create_result_aggregator() = 0;

  // Process a single work item on the current rank.
  virtual void process_item(const WorkItem& item,
                            IFeatureMapper& mapper,
                            const RunConfig& cfg) = 0;

  // Export local mapper state to a portable partial result that can be gathered to root.
  virtual PartialResult export_partial(const IFeatureMapper& mapper) = 0;

  // Merge a gathered partial into the hierarchical aggregator (root rank).
  virtual void merge_into_aggregator(const PartialResult& pr,
                                     IResultAggregator& agg) = 0;

  // Write final contract outputs (root rank).
  virtual void write_outputs(const IResultAggregator& agg,
                             const RunConfig& cfg) = 0;
};

} // namespace lacc::core
