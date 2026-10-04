#include "lacc/adt/HashTableChained.hpp"

#include <catch2/catch_test_macros.hpp>

#include <iostream>

using lacc::adt::HashTableChained;

TEST_CASE("HashTableChained put/get/contains basics") {
  HashTableChained<int> ht(7);

  REQUIRE(ht.size() == 0);
  REQUIRE_FALSE(ht.contains("a"));
  REQUIRE_FALSE(ht.get("a").has_value());

  ht.put("a", 1);
  REQUIRE(ht.size() == 1);
  REQUIRE(ht.contains("a"));
  REQUIRE(ht.get("a").has_value());
  REQUIRE(*ht.get("a") == 1);

  // overwrite
  ht.put("a", 9);
  REQUIRE(ht.size() == 1);
  REQUIRE(*ht.get("a") == 9);
}

TEST_CASE("HashTableChained increment inserts and updates") {
  HashTableChained<int> ht(5);

  REQUIRE(ht.increment("k") == 1);
  REQUIRE(ht.size() == 1);
  REQUIRE(*ht.get("k") == 1);

  REQUIRE(ht.increment("k", 4) == 5);
  REQUIRE(ht.size() == 1);
  REQUIRE(*ht.get("k") == 5);
}

TEST_CASE("HashTableChained handles many keys") {
  HashTableChained<int> ht(11);
  for (int i = 0; i < 1000; ++i) {
    ht.increment("key_" + std::to_string(i));
  }
  REQUIRE(ht.size() == 1000);
  for (int i = 0; i < 1000; ++i) {
    const auto v = ht.get("key_" + std::to_string(i));
    REQUIRE(v.has_value());
    REQUIRE(*v == 1);
  }
}
