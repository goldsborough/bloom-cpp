#include <bloom/filter.hpp>
#include <bloom/static-filter.hpp>

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>
#include <vector>

// NOLINTNEXTLINE
TEST(TestStaticFilter, SizeAndHashCountAsExpected) {
  {
    Bloom::StaticFilter<32, 3> filter;
    ASSERT_EQ(filter.size(), 32u);
    ASSERT_EQ(filter.hash_count(), 3u);
  }
  {
    Bloom::StaticFilter<12345, 678> filter;
    ASSERT_EQ(filter.size(), 12345u);
    ASSERT_EQ(filter.hash_count(), 678u);
  }
}

// NOLINTNEXTLINE
TEST(TestStaticFilter, TestQueryAlwaysReturnsTrueForInsertedKeys) {
  Bloom::StaticFilter<123, 3> filter;
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
  Bloom::StaticFilter<123, 2, Hasher> filter(Hasher(1), Hasher(2));
  filter.put(0);
  ASSERT_TRUE(filter.query(0));
  ASSERT_FALSE(filter.query(1));  // one collision
  ASSERT_FALSE(filter.query(2));  // no collision
}

// NOLINTNEXTLINE
TEST(TestStaticFilter, TestQueryReturnsTrueForMissingKeysWhenAllHashesCollide) {
  struct Hasher {
    uint32_t seed;
    explicit Hasher(uint32_t seed) : seed(seed) {}
    uint32_t operator()(Bloom::Slice /*unused*/) const noexcept { return seed; }
  };
  Bloom::StaticFilter<123, 2, Hasher> filter(Hasher(1), Hasher(2));
  filter.put(0);
  ASSERT_TRUE(filter.query(0));
  ASSERT_TRUE(filter.query(1));  // two collisions
}

// NOLINTNEXTLINE
TEST(TestStaticFilter, TestClearUnsetsAllBits) {
  Bloom::StaticFilter<10, 1> filter;
  filter.put(0);
  filter.put(1);
  filter.put(2);
  ASSERT_TRUE(filter.query(0));
  ASSERT_TRUE(filter.query(1));
  ASSERT_TRUE(filter.query(2));
  filter.clear();
  ASSERT_FALSE(filter.query(0));
  ASSERT_FALSE(filter.query(1));
  ASSERT_FALSE(filter.query(2));
}

// NOLINTNEXTLINE
TEST(TestFilter, SizeAndHashCountAsExpectedForExplicitOptions) {
  {
    Bloom::Filter filter(32, 3);
    ASSERT_EQ(filter.size(), 32u);
    ASSERT_EQ(filter.hash_count(), 3u);
  }
  {
    Bloom::Filter filter(12345, 678);
    ASSERT_EQ(filter.size(), 12345u);
    ASSERT_EQ(filter.hash_count(), 678u);
  }
}

// NOLINTNEXTLINE
TEST(TestFilter, SizeAndHashCountAsExpectedForExpectedCount) {
  {
    // 100/1 * ln(2) is approx. 69.3 and ceil(69.3) = 70
    Bloom::Filter filter(Bloom::Options::ForExpectedCount(100, 1));
    ASSERT_EQ(filter.size(), 100u);
    ASSERT_EQ(filter.hash_count(), 70u);
  }
  {
    // 6/2 * ln(2) is approx. 2.1 and ceil(2.1) = 3
    Bloom::Filter filter(Bloom::Options::ForExpectedCount(6, 2));
    ASSERT_EQ(filter.size(), 6u);
    ASSERT_EQ(filter.hash_count(), 3u);
  }
  {
    // 34/2 * ln(2) is approx. 11.8 and ceil(11.8) = 12
    Bloom::Filter filter(Bloom::Options::ForExpectedCount(34, 2));
    ASSERT_EQ(filter.size(), 34u);
    ASSERT_EQ(filter.hash_count(), 12u);
  }
}

// NOLINTNEXTLINE
TEST(TestFilter, SizeAndHashCountAsExpectedForFalsePositiveRate) {
  {
    // -log_2(0.5) = 1
    Bloom::Filter filter(Bloom::Options::ForFalsePositiveRate(100, 0.5));
    ASSERT_EQ(filter.size(), 100u);
    ASSERT_EQ(filter.hash_count(), 1u);
  }
  {
    // The size does not influence the hash count for a given fp rate.
    Bloom::Filter filter(Bloom::Options::ForFalsePositiveRate(12345, 0.5));
    ASSERT_EQ(filter.size(), 12345u);
    ASSERT_EQ(filter.hash_count(), 1u);
  }
  {
    // -log_2(0.1) = 3.3 and ceil(3.3) = 4
    Bloom::Filter filter(Bloom::Options::ForFalsePositiveRate(12345, 0.1));
    ASSERT_EQ(filter.size(), 12345u);
    ASSERT_EQ(filter.hash_count(), 4u);
  }
}

// NOLINTNEXTLINE
TEST(TestFilter, ThrowsWhenHashCountIsGreaterThanSize) {
  ASSERT_THROW([] { Bloom::Filter(5, 10); }(), std::invalid_argument);
}

// NOLINTNEXTLINE
TEST(TestOptions, ThrowsWhenHashCountIsGreaterThanSize) {
  ASSERT_THROW([] { Bloom::Options(5, 10); }(), std::invalid_argument);
}

// NOLINTNEXTLINE
TEST(TestFilter, TestQueryAlwaysReturnsTrueForInsertedKeys) {
  Bloom::Filter filter(32, 3);
  filter.put(1);
  filter.put("hello");

  std::vector<int> v = {1, 2, 3};
  filter.put(v);

  ASSERT_TRUE(filter.query(1));
  ASSERT_TRUE(filter.query("hello"));
  ASSERT_TRUE(filter.query(v));
}

// NOLINTNEXTLINE
TEST(TesFilter, TestQueryReturnsFalseForMissingKeysWhenNotAllHashesCollide) {
  struct Hasher {
    uint32_t seed;
    explicit Hasher(uint32_t seed) : seed(seed) {}
    uint32_t operator()(Bloom::Slice slice) const noexcept {
      return slice.data()[0] + seed;
    }
  };
  Bloom::Filter filter(10, {Hasher(1), Hasher(2)});
  filter.put(0);
  ASSERT_TRUE(filter.query(0));
  ASSERT_FALSE(filter.query(1));  // one collision
  ASSERT_FALSE(filter.query(2));  // no collision
}

// NOLINTNEXTLINE
TEST(TestFilter, TestQueryReturnsTrueForMissingKeysWhenAllHashesCollide) {
  struct Hasher {
    uint32_t seed;
    explicit Hasher(uint32_t seed) : seed(seed) {}
    uint32_t operator()(Bloom::Slice /* unused */) const noexcept {
      return seed;
    }
  };
  Bloom::Filter filter(10, {Hasher(1), Hasher(2)});
  filter.put(0);
  ASSERT_TRUE(filter.query(0));
  ASSERT_TRUE(filter.query(1));  // two collisions
}

// NOLINTNEXTLINE
TEST(TestFilter, TestClearUnsetsAllBits) {
  Bloom::Filter filter(10, 1);
  filter.put(0);
  filter.put(1);
  filter.put(2);
  ASSERT_TRUE(filter.query(0));
  ASSERT_TRUE(filter.query(1));
  ASSERT_TRUE(filter.query(2));
  filter.clear();
  ASSERT_FALSE(filter.query(0));
  ASSERT_FALSE(filter.query(1));
  ASSERT_FALSE(filter.query(2));
}

// NOLINTNEXTLINE
TEST(TestFilter, TestSliceableIsSpecializedForExpectedTypes) {
  Bloom::Filter filter(1000, 1);

  filter.put(static_cast<char>(1));
  filter.put(static_cast<unsigned char>(2));
  filter.put(static_cast<short>(3));
  filter.put(static_cast<unsigned short>(4));
  filter.put(static_cast<int>(5));
  filter.put(static_cast<unsigned int>(6));
  filter.put(static_cast<long>(7));
  filter.put(static_cast<unsigned long>(8));
  filter.put(static_cast<long long>(9));
  filter.put(static_cast<unsigned long long>(10));
  filter.put(static_cast<float>(11));
  filter.put(static_cast<double>(12));

  ASSERT_TRUE(filter.query(static_cast<char>(1)));
  ASSERT_TRUE(filter.query(static_cast<unsigned char>(2)));
  ASSERT_TRUE(filter.query(static_cast<short>(3)));
  ASSERT_TRUE(filter.query(static_cast<unsigned short>(4)));
  ASSERT_TRUE(filter.query(static_cast<int>(5)));
  ASSERT_TRUE(filter.query(static_cast<unsigned int>(6)));
  ASSERT_TRUE(filter.query(static_cast<long>(7)));
  ASSERT_TRUE(filter.query(static_cast<unsigned long>(8)));
  ASSERT_TRUE(filter.query(static_cast<long long>(9)));
  ASSERT_TRUE(filter.query(static_cast<unsigned long long>(10)));
  ASSERT_TRUE(filter.query(static_cast<float>(11)));
  ASSERT_TRUE(filter.query(static_cast<double>(12)));
}
