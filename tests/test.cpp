#include <bloom/filter.hpp>

#include <gtest/gtest.h>

TEST(TestStaticBloomFilter, BasicInterface) {
  Bloom::Filter<32, 3> filter;
  filter.put(123);
  ASSERT_TRUE(filter.maybe_contains(123));
}

// TEST_CASE(TestBloom, TestMayContainsAlwaysReturnsTrueForInsertedKeys) {
//
// }
