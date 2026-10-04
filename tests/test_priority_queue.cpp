#include "../third_party/Catch2-3.5.4/Catch2-3.5.4/src/catch2/catch_test_macros.hpp"

#include "../include/lacc/adt/PriorityQueue.hpp"

#include <functional>

using lacc::adt::PriorityQueue;

TEST_CASE("PriorityQueue: max-heap ordering with std::less") {
  PriorityQueue<int, std::less<int>> pq;

  pq.push(3);
  pq.push(10);
  pq.push(5);
  pq.push(7);

  REQUIRE(pq.top() == 10);
  pq.pop();
  REQUIRE(pq.top() == 7);
  pq.pop();
  REQUIRE(pq.top() == 5);
  pq.pop();
  REQUIRE(pq.top() == 3);
}

TEST_CASE("PriorityQueue: min-heap ordering with std::greater") {
  PriorityQueue<int, std::greater<int>> pq;

  pq.push(3);
  pq.push(10);
  pq.push(5);
  pq.push(7);

  REQUIRE(pq.top() == 3);
  pq.pop();
  REQUIRE(pq.top() == 5);
  pq.pop();
  REQUIRE(pq.top() == 7);
  pq.pop();
  REQUIRE(pq.top() == 10);
}

TEST_CASE("PriorityQueue: Top-K pattern keeps K largest values using a min-heap") {
  constexpr std::size_t K = 3;
  PriorityQueue<int, std::greater<int>> pq; // min-heap: smallest element at top

  auto offer = [&](int v) {
    if (pq.size() < K) {
      pq.push(v);
    } else if (v > pq.top()) {
      pq.pop();
      pq.push(v);
    }
  };

  // Feed values: top-3 largest should be {10, 7, 6}.
  for (int v : {3, 10, 5, 7, 1, 6}) {
    offer(v);
  }

  REQUIRE(pq.size() == K);
  // In a min-heap holding the top-3 largest values, the smallest of them is on top.
  REQUIRE(pq.top() == 6);
}
