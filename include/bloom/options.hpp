#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace Bloom {

/// A configuration struct for `Filter`.
///
/// Encompasses a size (of the filter) and a hash count (number of hash
/// functions). An `Options` object may be constructed directly from a size and
/// hash count, or via one of the factory methods that compute the optimal
/// `Options` for particular parameters of the bloom filter to be constructed,
/// for example the false positive rate.
struct Options {
  /// Constructs the `Options` from a filter size and hash count.
  Options(size_t size, size_t hash_count) : size(size), hash_count(hash_count) {
    if (hash_count > size) {
      throw std::invalid_argument(
          "the number of hash functions must not be greater than the "
          "size of the bloom filter");
    }
  }

  /// Computes the optimal `Options` for a filter size and expected false
  /// positive rate.
  ///
  /// The hash count will be computed such that the proportion of
  /// keys that have not been added to the bloom filter, but are still reported
  /// as inserted, matches the given false positive rate in expectation.
  static Options ForFalsePositiveRate(size_t size, double false_positive_rate) {
    const double optimal_hash_count = -std::log2(false_positive_rate);
    return {size, static_cast<size_t>(std::ceil(optimal_hash_count))};
  }

  /// Computes the optimal `Options` for a filter size and expected number of
  /// inserted entries.
  static Options ForExpectedCount(size_t size,
                                  size_t expected_number_of_insertions) {
    static const double kLn2 = std::log(2.0);
    const double optimal_hash_count =
        (size / static_cast<double>(expected_number_of_insertions)) * kLn2;
    return {size, static_cast<size_t>(std::ceil(optimal_hash_count))};
  }

  /// The size (number of entries) to use for the bloom filter.
  size_t size;

  /// The number of hash functions to use in the bloom filter.
  size_t hash_count;
};
}  // namespace Bloom
