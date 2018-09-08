#include <bloom/filter.hpp>

#include <gtest/gtest.h>

// NOLINTNEXTLINE
TEST(TestStaticBloomFilter, BasicInterface) {
  Bloom::Filter<32, 3> filter;
  filter.put(2);
  ASSERT_TRUE(filter.maybe_contains(123));
  ASSERT_TRUE(filter.maybe_contains(5));
  ASSERT_TRUE(filter.maybe_contains(1));
  ASSERT_TRUE(filter.maybe_contains(2));
}

// TEST_CASE(TestBloom, TestMayContainsAlwaysReturnsTrueForInsertedKeys) {
//{}
// }
