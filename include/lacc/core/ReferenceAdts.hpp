#pragma once

// Reference implementations of required ADT-shaped interfaces.
//
// Instructor/framework note:
//   These types exist so the starter framework can run end-to-end before
//   students provide their own ADTs. They are NOT intended as the student
//   submission, but they are kept as part of the repo to support demos and
//   incremental scaffolding.

#include "lacc/core/IFeatureMapper.hpp"
#include "lacc/core/IResultAggregator.hpp"

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace lacc::core {

class ReferenceFeatureMapper final : public IFeatureMapper {
public:
  void clear() override { m_.clear(); }
  std::size_t size() const override { return m_.size(); }

  void add(std::string_view key, std::int64_t delta) override {
    m_[std::string(key)] += delta;
  }

  bool get(std::string_view key, std::int64_t& out) const override {
    auto it = m_.find(std::string(key));
    if (it == m_.end()) return false;
    out = it->second;
    return true;
  }

  const std::unordered_map<std::string, std::int64_t>& data() const { return m_; }

private:
  std::unordered_map<std::string, std::int64_t> m_;
};

class ReferenceResultAggregator final : public IResultAggregator {
public:
  void add_leaf_value(std::string_view path, std::int64_t value) override;
  void aggregate() override;
  std::vector<Row> to_rows_preorder() const override;

private:
  struct Node {
    std::string name;
    std::string path;
    std::int64_t value = 0;
    bool is_leaf = false;
    std::map<std::string, std::unique_ptr<Node>> children; // ordered for determinism
  };

  Node root_{.name="ROOT", .path="ROOT"};

  static std::vector<std::string> split_path(std::string_view path);
  static void preorder(const Node& n, std::size_t level, std::vector<Row>& out);
  static std::int64_t aggregate_rec(Node& n);
};

} // namespace lacc::core
