#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace lacc::core {

// Hierarchical result aggregator interface (conceptually a tree).
class IResultAggregator {
public:
  struct Row {
    std::string taxon_path;
    std::string taxon_name;
    std::int64_t value = 0;
    std::size_t level = 0;
    bool is_leaf = false;
  };

  virtual ~IResultAggregator() = default;

  // Add a value at a leaf node addressed by an absolute path.
  virtual void add_leaf_value(std::string_view path, std::int64_t value) = 0;

  // Perform aggregation (e.g., internal node sums). Must be deterministic.
  virtual void aggregate() = 0;

  // Deterministic preorder traversal used for output writing.
  virtual std::vector<Row> to_rows_preorder() const = 0;
};

} // namespace lacc::core
