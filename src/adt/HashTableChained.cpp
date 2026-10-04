#include "lacc/adt/HashTableChained.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <iostream>

namespace lacc::adt {
#include "lacc/adt/HashTableChained.hpp"

#include <cstdint>
#include <stdexcept>

// Constructor
template <typename V> // what does this do??? look it up, hint: what if you want to hash different classes?
HashTableChained<V>::HashTableChained(std::size_t bucket_count)
  : buckets_(bucket_count, nullptr), n_entries_(0) {
  if (bucket_count == 0) {
    throw std::invalid_argument("bucket_count must be > 0");
  }
}

// Destructor
template <typename V>
HashTableChained<V>::~HashTableChained() {
  free_all();
}

// Insert or update (stub)
template <typename V>
void HashTableChained<V>::put(const Key& key, const V& value) {
  size_t idx_ = idx(key);
  Node *currNode = buckets_[idx_];
  if (!currNode) {
    currNode = new Node{key, value}; // don't use currNode because it will be dragged away from buckets_
    buckets_[idx_] = currNode;
    n_entries_++;
    return;
  }
  while (currNode) {
    if (currNode->key.compare(key) == 0) {  // update value if node already there
      currNode->value = value;
    } else if (!currNode->next) {   // add new node if node not already there
      currNode->next = new Node{key, value};
      n_entries_++;
      break;
    }

    currNode = currNode->next;
  }
}

// Check existence (stub)
template <typename V>
bool HashTableChained<V>::contains(const Key& key) const {
  // Check if an element is contained in the hashtable
  // quick test
  size_t idx_ = idx(key);
  Node *head = buckets_[idx_];
  if (!head) {
    return false;
  }
  if (head->key.compare(key) == 0) {
    return true;
  }
  while (head->next) {
    if (head->key.compare(key) == 0) {
      return true;
    }
    head = head->next;
  }
  return false;
}

// Retrieve value (stub)
// NOTE: std::optional (converted to bool) returns true if it contains a value, and false if not
template <typename V>
std::optional<V> HashTableChained<V>::get(const Key& key) const {
  // Retrieve the value from the given key in the hashtable
  size_t idx_ = idx(key);
  std::optional<V> value;
  Node *head = buckets_[static_cast<int>(idx_)];
  while (head) {
    if (head->key.compare(key) == 0) {
      value = head->value;
      break;
    }
    head = head->next;
  }
  return value ? value : std::nullopt;
}

// Increment helper (stub)
template <typename V>
V HashTableChained<V>::increment(const Key& key, const V& delta) {
  Node* n = find_node(key);
  if (n) {
    n->value += delta;
    return n->value;
  }
  const std::size_t i = idx(key);
  Node* head = buckets_[static_cast<int>(i)];
  Node* nn = new Node{key, delta, head};
  buckets_[i] = nn;
  ++n_entries_;
  return nn->value;
}

// Enumerate entries (stub)
template <typename V>
std::vector<std::pair<Key, V>> HashTableChained<V>::entries() const {
  std::vector<std::pair<Key, V>> out;
  out.reserve(n_entries_);
  for (Node* head : buckets_) {
    for (Node* cur = head; cur != nullptr; cur = cur->next) {
      out.emplace_back(cur->key, cur->value);
    }
  }
  return out;
}

template <typename V>
std::size_t HashTableChained<V>::idx(const Key& key) const {
  // This is a commonly used hash function but write your own, there are many examples of different ways to do it
  std::uint64_t h = 1469598103934665603ull;
  for (unsigned char c : key) {
    h ^= static_cast<std::uint64_t>(c);
    h *= 1099511628211ull;
  }
  return static_cast<std::size_t>(h % buckets_.size());
}

template <typename V>
typename HashTableChained<V>::Node* HashTableChained<V>::find_node(const Key& key) const {
  const std::size_t i = idx(key);
  Node* cur = buckets_[static_cast<int>(i)];
  while (cur) {
    if (cur->key == key) return cur;
    cur = cur->next;
  }
  return nullptr;
}

// Explicit instantiations used in the demo
template class HashTableChained<int>;
template class HashTableChained<double>;

template <typename V>
void HashTableChained<V>::free_all() {
  // Instructor-provided cleanup so starter objects can be destroyed safely.
  for (Node*& head : buckets_) {
    Node* cur = head;
    while (cur) {
      Node* nxt = cur->next;
      delete cur;
      cur = nxt;
    }
    head = nullptr;
  }
  n_entries_ = 0;
}

/* template class HashTableChained<int>;
template class HashTableChained<double>; */

} // namespace lacc::adt
