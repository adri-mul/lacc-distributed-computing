# Priority Queue (Heap) in LACC: Keeping the Top-K

This repo includes a small **PriorityQueue** ADT implemented with a binary heap:

- Header: `include/lacc/adt/PriorityQueue.hpp`
- Tests: `tests/test_priority_queue.cpp`

The main reason we add this ADT in LACC is that **all four student projects** need
to keep the *Top-K* best results without sorting an entire dataset.

## The Top-K pattern (keep the K largest scores)

Use a **min-heap** of size `K`:

```cpp
#include "lacc/adt/PriorityQueue.hpp"
#include <functional>

constexpr std::size_t K = 10;
// min-heap: smallest element is on top
lacc::adt::PriorityQueue<double, std::greater<double>> pq;

auto offer = [&](double score) {
  if (pq.size() < K) {
    pq.push(score);
  } else if (score > pq.top()) {
    pq.pop();
    pq.push(score);
  }
};
```

Why this works:

- The heap holds **at most K elements**.
- The heap’s `top()` is the *smallest of the kept elements*.
- Any new value that does not beat `top()` is discarded.

Time complexity:

- `offer(...)` is `O(log K)` per accepted element.
- Much cheaper than sorting `O(N log N)` when `K << N`.

## Where this shows up in the project

### Physics (P1/P2)
Keep the Top-K **SNR peaks** while scanning for local maxima.

### Biology (B1/B2)
Keep the Top-K **k-mers** by count (B1) or enrichment score (B2).

In both tracks, the ADT is identical; only the interpretation of “score” differs.