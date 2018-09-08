#pragma once

#include <bloom/hash.hpp>
#include <bloom/slice.hpp>

#include <array>
#include <bitset>
#include <cstddef>
#include <initializer_list>

namespace Bloom {
// TODO: provide functions for computing optimal K given N and M .....
template <size_t N, size_t k, typename Hasher = DefaultHasher>
struct Filter {
 public:
  static_assert(k <= N,
                "the number of hash functions must not be greater than the "
                "size of the bloom filter");

  using HasherType = Hasher;

  /// Default-constructs the filter and each of the `K` `Hasher` objects.
  Filter() = default;

  /// Constructs the `Filter` with an initializer list of `Hasher` objects.
  template <typename... Hashers>
  explicit Filter(Hashers&&... hashers) noexcept
  : hashers_({{std::forward<Hashers>(hashers)...}}) {}

  /// Inserts the given `key` into the bloom filter.
  ///
  /// Inserting means passing a key `x` through every hash function `h_k` with
  /// `k` in `1..K` and setting the bit in the position returned by `h_k(x)` to
  /// one.
  void put(Slice slice) noexcept {
    for (const auto& hasher : hashers_) {
      const auto index = hasher(slice) % N;
      bits_.set(index);
    }
  }

  /// Returns `true` if the given `key` has possibly been inserted in the
  /// bloom filter.
  ///
  /// A particular key `x` is assumed to be present if *for every* hash function
  /// `h_k` with `k` in `1..K` the bit in the position `h_k(x)` the function
  /// hashes the key to is *set* (one).
  bool query(Slice key) const noexcept {
    for (const auto& hasher : hashers_) {
      const auto index = hasher(key) % N;
      if (!bits_.test(index)) {
        return false;
      }
    }
    return true;
  }

  /// Clears all entries in the bloom filter.
  void clear() const noexcept { bits_.reset(); }

  /// Returns the size (`N`; number of bits) of the bloom filter.
  size_t size() const noexcept { return N; }

  /// Returns the number of hash functions (`k`) used in `put()` and `query()`
  /// operations.
  size_t hash_count() const noexcept { return k; }

 private:
  std::array<Hasher, k> hashers_;
  std::bitset<N> bits_;
};
}  // namespace Bloom
