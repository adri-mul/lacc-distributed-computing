# ADT Contract (Student Deliverable)

This repository intentionally treats the data structures as a **contracted deliverable**:
the rest of the framework depends on the *behavior* of these ADTs, not on any specific
implementation strategy.

The automated tests will validate these contracts.

## 1) PriorityQueue<T, Compare> (Binary Heap)

Header: `include/lacc/adt/PriorityQueue.hpp`

### Required Behavior

* `empty()`, `size()` must reflect the number of stored elements.
* `top()` returns the element with **highest priority** according to `Compare` and throws
  on empty.
* `push()` inserts an element and preserves the heap invariant.
* `pop()` removes the current `top()` element and preserves the heap invariant; throws on empty.

### Comparator Semantics (matches `std::priority_queue`)

`Compare(a, b)` returns `true` if `a` has **lower priority** than `b`.

Examples:

* `Compare = std::less<T>` produces a **max-heap** (largest element is on top).
* `Compare = std::greater<T>` produces a **min-heap** (smallest element is on top).

### Complexity Expectations

* `push`: `O(log N)`
* `pop`: `O(log N)`
* `top`: `O(1)`

## 2) HashTableChained<V> (Separate Chaining)

Header: `include/lacc/adt/HashTableChained.hpp`
Implementation: `src/adt/HashTableChained.cpp`

### Required Behavior

* Keys are strings (`using Key = std::string`).
* Collisions are handled via **separate chaining** (linked nodes per bucket).
* `put(key, value)` inserts a new key or overwrites the stored value for an existing key.
* `contains(key)` returns whether the key exists.
* `get(key)` returns `std::optional<V>` (empty if key not present).
* `increment(key, delta)`:
  * If key exists: adds `delta` to the stored value and returns the new value.
  * If key does not exist: inserts the key with initial value `delta` and returns `delta`.
* `size()` returns the number of distinct keys.
* `entries()` returns all `(key, value)` pairs currently stored.

### Determinism

`entries()` must be **deterministic** for the same sequence of operations.
The framework may use the resulting list for reproducible outputs.

### Complexity Expectations

Assuming a reasonable bucket count and hash distribution:

* Average-case `put/contains/get/increment`: `O(1)`
* Worst-case: `O(N)` (all keys in one chain)

## 3) "No STL Substitution" Policy (when enabled by the course)

If your instructor enables checks for forbidden substitutions, you may be prohibited from
using STL containers (e.g., `std::unordered_map`) as a drop-in replacement for the above ADTs.
The goal is for you to implement and reason about invariants, memory ownership, and complexity.
