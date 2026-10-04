#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace lacc::core {

// Feature mapper interface (conceptually a hash table).
class IFeatureMapper {
public:
  virtual ~IFeatureMapper() = default;

  virtual void clear() = 0;
  virtual std::size_t size() const = 0;

  // Add delta to the feature key (insert if missing).
  virtual void add(std::string_view key, std::int64_t delta) = 0;

  // Retrieve value for key; returns false if key is not present.
  virtual bool get(std::string_view key, std::int64_t& out) const = 0;
};

} // namespace lacc::core
