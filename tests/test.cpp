#include <bloom/filter.hpp>

#include <gtest/gtest.h>

#include <string>
#include <vector>

// NOLINTNEXTLINE
TEST(TestStaticFilter, SizeAndHashCountAsExpected) {
  {
    Bloom::Filter<32, 3> filter;
    ASSERT_EQ(filter.size(), 32u);
    ASSERT_EQ(filter.hash_count(), 3u);
  }
  {
    Bloom::Filter<12345, 678> filter;
    ASSERT_EQ(filter.size(), 12345u);
    ASSERT_EQ(filter.hash_count(), 678u);
  }
}

// NOLINTNEXTLINE
TEST(TestStaticFilter, TestQueryAlwaysReturnsTrueForInsertedKeys) {
  Bloom::Filter<123, 3> filter;
  filter.put(1);
  filter.put("hello");

  std::vector<int> v = {1, 2, 3};
  filter.put(v);

  ASSERT_TRUE(filter.query(1));
  ASSERT_TRUE(filter.query("hello"));
  ASSERT_TRUE(filter.query(v));
}

// NOLINTNEXTLINE
TEST(TestStaticFilter,
     TestQueryReturnsFalseForMissingKeysWhenNotAllHashesCollide) {
  struct Hasher {
    uint32_t seed;
    explicit Hasher(uint32_t seed) : seed(seed) {}
    uint32_t operator()(Bloom::Slice slice) const noexcept {
      return slice.data()[0] + seed;
    }
  };
  Bloom::Filter<123, 2, Hasher> filter(Hasher(1), Hasher(2));
  filter.put(0);
  ASSERT_TRUE(filter.query(0));
  ASSERT_FALSE(filter.query(1));  // one collision
  ASSERT_FALSE(filter.query(2));  // no collision
}

// NOLINTNEXTLINE
TEST(TestStaticFilter,
     TestQueryReturnsTrueForMissingKeysWhenAllHashesCollide) {
  struct Hasher {
    uint32_t seed;
    explicit Hasher(uint32_t seed) : seed(seed) {}
    uint32_t operator()(Bloom::Slice /*unused*/) const noexcept {
      return seed;
    }
  };
  Bloom::Filter<123, 2, Hasher> filter(Hasher(1), Hasher(2));
  filter.put(0);
  ASSERT_TRUE(filter.query(0));
  ASSERT_TRUE(filter.query(1));  // two collisions
}
