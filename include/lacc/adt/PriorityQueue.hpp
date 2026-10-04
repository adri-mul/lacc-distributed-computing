#pragma once

// LACC Priority Queue ADT (Binary Heap)
//
// Phase 1.4 student starter:
//   This ADT intentionally compiles but is not fully implemented.
//   Students must complete the heap operations while preserving the public API.
//
// Required representation:
//   - Store the heap in a contiguous array named data_.
//   - Parent index of i is (i - 1) / 2.
//   - Left child of i is 2*i + 1.
//   - Right child of i is 2*i + 2.
//
// Comparator semantics:
//   - Compare(a, b) returns true if a has LOWER priority than b.
//   - std::less<T> gives a max-heap.
//   - std::greater<T> gives a min-heap.
//
// Student TODOs:
//   - push(const T&) inserts a value and restores the heap invariant.
//   - push(T&&) inserts a moved value and restores the heap invariant.
//   - pop() removes the root and restores the heap invariant.
//   - sift_up() and sift_down() perform the heap repairs.
//
// Determinism note:
//   The raw() array is for testing and teaching only. Heap order is not sorted
//   order, but your behavior must still be deterministic for the same input.

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>
#include <iostream>

namespace lacc::adt {

template <typename T, typename Compare = std::less<T>>
class PriorityQueue {
public:
  PriorityQueue() = default;
  explicit PriorityQueue(Compare comp) : comp_(std::move(comp)) {}

  [[nodiscard]] bool empty() const noexcept { return data_.empty(); }
  [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }

  [[nodiscard]] const T& top() const {
    if (data_.empty()) {
      throw std::runtime_error("PriorityQueue::top() called on empty queue");
    }
    return data_.front();
  }

  void push(const T& value) {
    // STUDENT TODO:
    // 1. Append value to the end of data_.
    // 2. Call sift_up() on the new index.
    // 3. Preserve the heap invariant defined by Compare.
    data_.push_back(value);
    sift_up(data_.size()-1);
  }

  void push(T&& value) {
    // STUDENT TODO:
    // Same logic as the const-reference overload, but move the value into data_.
    data_.push_back(std::move(value));
    sift_up(data_.size()-1);
  }

  void pop() {
    if (data_.empty()) {
      throw std::runtime_error("PriorityQueue::pop() called on empty queue");
    }

    // STUDENT TODO:
    // Cases to handle:
    //   - one element: remove it directly
    //   - more than one element:
    //       a) swap root with last element
    //       b) remove the last element
    //       c) call sift_down(0)
    if (data_.size() < 2) {
      data_.clear();
    } else {
      std::swap(data_.back(), data_.front());
      data_.pop_back();
      sift_down(0);
    }
  }

  [[nodiscard]] const std::vector<T>& raw() const noexcept { return data_; }

private:
  std::vector<T> data_;
  Compare comp_{};

  static std::size_t parent(std::size_t i) { return (i - 1) / 2; }
  static std::size_t left(std::size_t i) { return 2 * i + 1; }
  static std::size_t right(std::size_t i) { return 2 * i + 2; }

  void sift_up(std::size_t i) {
    // STUDENT TODO:
    // While i is not the root:
    //   - compare the parent and child using comp_
    //   - swap when the parent has lower priority than the child
    //   - continue from the parent index after each swap
    while (i > 0 && comp_(data_[parent(i)], data_[i])) {
      std::swap(data_[parent(i)], data_[i]);
      i = parent(i);
    }
  }

  void sift_down(std::size_t i) {
    // STUDENT TODO:
    // Repeatedly choose the highest-priority element among:
    //   - current node i
    //   - left child (if present)
    //   - right child (if present)
    // Swap with the best child when needed, then continue from that child.
    size_t bestIndex = i;
    size_t l = left(i);
    if (l < data_.size() && comp_(data_[bestIndex], data_[l])) { 
      bestIndex = l;
    }
    size_t r = right(i);
    if (r < data_.size() && comp_(data_[bestIndex], data_[r])) {
      bestIndex = r;
    }

    if (i != bestIndex) {
        std::swap(data_[i], data_[bestIndex]);
        sift_down(bestIndex);
    }
  }
};

} // namespace lacc::adt
