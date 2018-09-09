#pragma once

#include <bloom/hash.hpp>
#include <bloom/options.hpp>
#include <bloom/slice.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace Bloom {

/// A bloom filter with runtime configurable size and hash count.
class Filter {
 public:
  using Hasher = std::function<uint32_t(Slice slice)>;

  /// Constructs a `Filter` with a size and a user provided iterator range of
  /// hash functions. The hash functions are copied into the filter and the
  /// distance between `hashers_begin` and `hashers_end` becomes the hash count.
  template <typename Iterator>
  Filter(size_t size, Iterator hashers_begin, Iterator hashers_end)
  : hashers_(hashers_begin, hashers_end), bits_(size) {
    if (hashers_.size() > size) {
      throw std::invalid_argument(
          "the number of hash functions must not be greater than the "
          "size of the bloom filter");
    }
  }

  /// Constructs a `Filter` with a size and a user provided list of hash
  /// functions.
  Filter(size_t size, std::initializer_list<Hasher> hashers)
  : Filter(size, hashers.begin(), hashers.end()) {}

  /// Constructs a `Filter` from the given options, using the
  /// `Bloom::DefaultHasher` as the `Hasher` type.
  explicit Filter(Options options) : bits_(options.size) {
    hashers_.reserve(options.hash_count);
    for (size_t h = 0; h < options.hash_count; ++h) {
      hashers_.emplace_back(DefaultHasher());
    }
  }

  // Constructs a `Filter` from a size and hash count.
  // Equivalent to constructing an `Options` object and using the constructor
  // from `Options`.
  Filter(size_t size, size_t hash_count) : Filter(Options(size, hash_count)) {}

  /// Inserts the given `key` into the bloom filter.
  ///
  /// Inserting means passing a key `x` through every hash function `h_k` with
  /// `k` in `1..K` and setting the bit in the position returned by `h_k(x)` to
  /// one.
  ///
  /// \complexity O(k)
  void put(Slice slice) {
    for (const auto& hasher : hashers_) {
      bits_[hasher(slice) % size()] = true;
    }
  }

  /// Returns `true` if the given `key` has possibly been inserted in the
  /// bloom filter.
  ///
  /// A particular key `x` is assumed to be present if *for every* hash function
  /// `h_k` with `k` in `1..K` the bit in the position `h_k(x)` the function
  /// hashes the key to is *set* (one).
  ///
  /// \complexity O(k)
  bool query(Slice key) const {
    return std::all_of(hashers_.begin(),
                       hashers_.end(),
                       [this, &key](const auto& hasher) {
                         return bits_[hasher(key) % size()];
                       });
  }

  /// Clears all entries in the bloom filter.
  /// \complexity O(N)
  void clear() { std::fill(bits_.begin(), bits_.end(), 0); }

  /// Returns the size (`N`; number of bits) of the bloom filter.
  size_t size() const noexcept { return bits_.size(); }

  /// Returns the number of hash functions (`k`) used in `put()` and `query()`
  /// operations.
  size_t hash_count() const noexcept { return hashers_.size(); }

 private:
  std::vector<Hasher> hashers_;
  std::vector<bool> bits_;
};
}  // namespace Bloom
