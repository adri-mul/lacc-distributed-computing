#pragma once
#include <cstddef>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace lacc::adt {

using Key = std::string;

template <typename V>
class IHashTable {
public:
  virtual ~IHashTable() = default;
  virtual void put(const Key& key, const V& value) = 0;
  virtual bool contains(const Key& key) const = 0;
  virtual std::optional<V> get(const Key& key) const = 0;
  virtual V increment(const Key& key, const V& delta = static_cast<V>(1)) = 0;
  virtual std::size_t size() const = 0;
  virtual std::vector<std::pair<Key, V>> entries() const = 0;
};

} // namespace lacc::adt
