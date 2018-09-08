#pragma once

#include <bloom/hash.hpp>
#include <bloom/slice.hpp>

#include <array>
#include <bitset>
#include <cstddef>

namespace Bloom {
template <size_t N, size_t K, typename Hasher = DefaultHasher>
struct Filter {
 public:
  Filter() = default;

  Filter(std::initializer_list<Hasher> hashers) noexcept : hashers_(hashers) {
  }

  void put(Slice slice) noexcept {
    for (const auto& hasher : hashers_) {
      const auto index = hasher(slice) % N;
      bits_.set(index);
    }
  }

  bool maybe_contains(Slice slice) const noexcept {
    for (const auto& hasher : hashers_) {
      const auto index = hasher(slice) % N;
      if (!bits_.test(index)) {
        return false;
      }
    }
    return true;
  }

 private:
  std::array<Hasher, K> hashers_;
  std::bitset<N> bits_;
};
}  // namespace Bloom
