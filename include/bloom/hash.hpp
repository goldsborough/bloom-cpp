#pragma once

#include <bloom/slice.hpp>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>

namespace Bloom {
namespace Detail {
template <typename T>
constexpr T rotate_left(T value, T amount) {
  constexpr auto word_size = std::numeric_limits<T>::digits;
  return (value << amount) | (value >> (word_size - amount));
}

inline uint32_t murmur3_32(const uint8_t* data, size_t size, uint32_t seed) {
  const uint32_t c1 = 0xcc9e2d51;
  const uint32_t c2 = 0x1b873593;
  const uint32_t r1 = 15;
  const uint32_t r2 = 13;

  uint32_t hash = seed;

  if (size >= 4) {
    const auto* four_byte_chunk = reinterpret_cast<const uint32_t*>(data);
    for (size_t i = 0, stop = size / 4; i < stop; ++i) {
      uint32_t k = *four_byte_chunk++;

      k *= c1;
      k = rotate_left(k, r1);
      k *= c2;

      hash ^= k;
      hash = rotate_left(hash, r2);
      hash = hash * 5 + 0xe6546b64;
    }
  }

  // if (size not divisble by 4)
  if ((size & 3u) != 0u) {
    uint32_t remaining_chunk = 0;
    const auto* remaining_bytes = data + size - 1;
    for (size_t i = 0, stop = size & 3u; i < stop; ++i) {
      remaining_chunk <<= 8;
      remaining_chunk |= static_cast<uint32_t>(*remaining_bytes--);
    }

    remaining_chunk *= c1;
    remaining_chunk = rotate_left(remaining_chunk, r1);
    remaining_chunk *= c2;
    hash ^= remaining_chunk;
  }

  hash ^= size;
  hash ^= hash >> 16;
  hash *= 0x85ebca6b;
  hash ^= hash >> 13;
  hash *= 0xc2b2ae35;
  hash ^= hash >> 16;

  return hash;
}
}  // namespace Detail

struct DefaultHasher {
  DefaultHasher() {
    std::random_device seed_device;
    std::mt19937 generator(seed_device());
    std::uniform_int_distribution<uint32_t> distribution;
    seed = distribution(generator);
  }

  explicit DefaultHasher(uint32_t seed) : seed(seed) {}

  uint32_t operator()(Slice slice) const noexcept {
    return Detail::murmur3_32(slice.data(), slice.size(), seed);
  }

  uint32_t seed;
};
}  // namespace Bloom
