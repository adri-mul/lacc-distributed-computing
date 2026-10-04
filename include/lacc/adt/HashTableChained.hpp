#pragma once
#include "lacc/adt/IHashTable.hpp"
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace lacc::adt {

template <typename V>
class HashTableChained final : public IHashTable<V> {
public:
  explicit HashTableChained(std::size_t bucket_count = 101);
  ~HashTableChained();

  void put(const Key& key, const V& value) override;
  bool contains(const Key& key) const override;
  std::optional<V> get(const Key& key) const override;
  V increment(const Key& key, const V& delta = static_cast<V>(1)) override;

  std::size_t size() const override { return n_entries_; }
  std::vector<std::pair<Key, V>> entries() const override;

private:
  struct Node {
    Key key;
    V value;
    Node* next{nullptr};
  };

  std::vector<Node*> buckets_;
  std::size_t n_entries_{0};

  std::size_t idx(const Key& key) const;
  Node* find_node(const Key& key) const;
  void free_all();
};

} // namespace lacc::adt
