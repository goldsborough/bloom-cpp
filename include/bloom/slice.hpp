#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

namespace Bloom {

template <typename T>
struct Sliceable;

class Slice {
 public:
  Slice(const uint8_t* data, size_t size) noexcept : data_(data), size_(size) {}

  template <typename T>
  // NOLINTNEXTLINE
  Slice(const T& value) noexcept : Slice(Sliceable<T>().to_slice(value)) {}

  const uint8_t* data() const noexcept { return data_; }
  size_t size() const noexcept { return size_; }

 private:
  const uint8_t* data_;
  size_t size_;
};

#define INTEGRAL_SLICEABLE(T)                                          \
  template <>                                                          \
  struct Sliceable<T> {                                                \
    Slice to_slice(const T& value) const noexcept {                    \
      return {reinterpret_cast<const uint8_t*>(&value), sizeof value}; \
    }                                                                  \
  }

INTEGRAL_SLICEABLE(char);
INTEGRAL_SLICEABLE(unsigned char);
INTEGRAL_SLICEABLE(short);
INTEGRAL_SLICEABLE(unsigned short);
INTEGRAL_SLICEABLE(int);
INTEGRAL_SLICEABLE(unsigned int);
INTEGRAL_SLICEABLE(long);
INTEGRAL_SLICEABLE(unsigned long);
INTEGRAL_SLICEABLE(long long);
INTEGRAL_SLICEABLE(unsigned long long);

#undef INTEGRAL_SLICEABLE

template <typename T, size_t N>
struct Sliceable<T[N]> {
  Slice to_slice(const T array[N]) const noexcept {
    return {reinterpret_cast<const uint8_t*>(array), N * sizeof(T)};
  }
};

template <typename T>
struct Sliceable<std::vector<T>> {
  Slice to_slice(const std::vector<T>& vector) const noexcept {
    return {reinterpret_cast<const uint8_t*>(vector.data()),
            vector.size() * sizeof(T)};
  }
};

template <typename T, size_t N>
struct Sliceable<std::array<T, N>> {
  Slice to_slice(const std::array<T, N>& array) const noexcept {
    return {reinterpret_cast<const uint8_t*>(array.data()), N * sizeof(T)};
  }
};

}  // namespace Bloom
