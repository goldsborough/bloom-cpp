#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

namespace Bloom {

/// A trait that may be specialized to enable construction of a `Slice` for a
/// particular type.
///
/// It should provide a `to_slice` method with the following signature:
/// ```cpp
/// Slice to_slice(const T&) const;
/// ```
template <typename T>
struct Sliceable;

/// A slice of raw bytes.
///
/// Simply a pointer to some raw bytes and the size of the buffer at that
/// memory address. To enable construction of a `Slice` for a new type, you
/// should specialize the `Sliceable` trait and implement the `to_slice` method.
/// For example:
///
/// ```cpp
/// struct Sliceable<MyType> {
///    Slice to_slice(const MyType& my_value) const {
///        return {my_value.data(), my_value.size_in_bytes()};
///    }
/// };
/// ```
class Slice {
 public:
  /// Constructs the `Slice` from a given data pointer and size.
  Slice(const uint8_t* data, size_t size) noexcept : data_(data), size_(size) {}

  /// Constructs a `Slice` from any type `T` for which the `Sliceable` trait has
  /// been specialized.
  template <typename T>
  Slice(const T& value) noexcept(  // NOLINT
      noexcept(Sliceable<T>().to_slice(value)))
  : Slice(Sliceable<T>().to_slice(value)) {}  // NOLINT

  /// Returns the data pointer.
  const uint8_t* data() const noexcept { return data_; }

  /// Returns the size.
  size_t size() const noexcept { return size_; }

 private:
  const uint8_t* data_;
  size_t size_;
};

#define NUMERIC_SLICEABLE(T)                                           \
  /** Specialization of `Sliceable` for `T`.  */                       \
  template <>                                                          \
  struct Sliceable<T> {                                                \
    Slice to_slice(const T& value) const noexcept {                    \
      return {reinterpret_cast<const uint8_t*>(&value), sizeof value}; \
    }                                                                  \
  }

NUMERIC_SLICEABLE(char);
NUMERIC_SLICEABLE(unsigned char);
NUMERIC_SLICEABLE(short);
NUMERIC_SLICEABLE(unsigned short);
NUMERIC_SLICEABLE(int);
NUMERIC_SLICEABLE(unsigned int);
NUMERIC_SLICEABLE(long);
NUMERIC_SLICEABLE(unsigned long);
NUMERIC_SLICEABLE(long long);
NUMERIC_SLICEABLE(unsigned long long);
NUMERIC_SLICEABLE(float);
NUMERIC_SLICEABLE(double);

#undef NUMERIC_SLICEABLE

/// Specialization of `Sliceable` for all built-in array types.
template <typename T, size_t N>
struct Sliceable<T[N]> {
  Slice to_slice(const T array[N]) const noexcept {
    return {reinterpret_cast<const uint8_t*>(array), N * sizeof(T)};
  }
};

/// Specialization of `Sliceable` for all `std::vector` types.
template <typename T>
struct Sliceable<std::vector<T>> {
  Slice to_slice(const std::vector<T>& vector) const noexcept {
    return {reinterpret_cast<const uint8_t*>(vector.data()),
            vector.size() * sizeof(T)};
  }
};

/// Specialization of `Sliceable` for all `std::array` types.
template <typename T, size_t N>
struct Sliceable<std::array<T, N>> {
  Slice to_slice(const std::array<T, N>& array) const noexcept {
    return {reinterpret_cast<const uint8_t*>(array.data()), N * sizeof(T)};
  }
};

}  // namespace Bloom
