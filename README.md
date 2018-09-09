# :mag: bloom-cpp

<p align="center">
  A header-only <a href="https://en.wikipedia.org/wiki/Bloom_filter">bloom filter</a> implementation in C++14.
  <br><br>
  <img alt="license" src="https://img.shields.io/github/license/mashape/apistatus.svg"/>
  <a href="https://travis-ci.org/goldsborough/bloom-cpp"><img alt="build status" src="https://travis-ci.org/goldsborough/bloom-cpp.svg?branch=master"/></a>
</p>

## API

A `Bloom::Filter` is parameterized by a size `n` and the number of hash functions `k` to use for each insertion or query operation. Use `put(key)` to add a value to the filter and `query(key)` to test for membership.

```cpp
#include <bloom/filter.h>

// 32 bits, 3 hash functions.
Bloom::Filter filter(32, 3);

filter.put(42);
filter.put("string");

assert(filter.query(42)); // guaranteed
assert(filter.query("string")); // guaranteed
assert(filter.query(123.456)); // maybe!

filter.clear(); // unset all entries
assert(!filter.query(42));
```

Notice that the filter is not parameterized by any (template) type, since a bloom filter generally
does not _store_ any values. Instead, `put()` and `query()` both take a `Bloom::Slice` -- a simple
abstraction over a blob of bytes, i.e. a pointer and a size. Specialize the `Bloom::Sliceable` trait
for your own type to make it usable throughout the library:

```cpp
namespace Bloom {
template<> struct Sliceable<MyType> {
  Slice to_slice(const MyType& value) const {
    return {value.data(), value.size_in_bytes()};
  }
};
} // namespace Bloom

Bloom::Filter filter(32, 3);
filter.put(MyType(...));
filter.query(MyType(...));
```

Instead of passing the number of hash functions explicitly, you can also use one of
`Bloom::Options` factory methods to compute the optimal number given either an expected false
positive rate, or expected number of inserted values:

```cpp
Bloom::Filter filter1(Bloom::Options::ForFalsePositiveRate(/*size=*/100, /*fp=*/0.2)); // k = 3
Bloom::Filter filter2(Bloom::Options::ForExpectedCount(/*size=*/100, /*expected_count=*/20)); // k = 4
```

Finally, the library also provides `Bloom::StaticFilter`, a compile-time version implementation of `Bloom::Filter` that takes the size and hash count as (non-type) template parameters. `Bloom::StaticFilter` does not incur any heap allocations for its internal storage. The API is the same as `Bloom::Filter`.

```cpp
Bloom::StaticFilter<32, 3> filter;
filter.put(123);
filter.query("string");
```

## Documentation

The documentation for this project can be built by running `doxygen` from within the `docs/` folder. This will generate a `build/html` folder that contains the doxygen HTML output.

## Building

The library is header only, so you can simply point your include path at the `include/` folder.

If you are using CMake, you can also link against the `bloom` target, which has the correct include paths configured in its interface properties. For example:

```cmake
add_executable(my-target)
add_subdirectory(third-party/bloom-cpp)
target_link_libraries(my-target bloom)
```

### Developing

If you would like to contribute to this library, you will likely want to build the tests. For this, run the following commands starting at the project root:

```sh
mkdir build && cd build
cmake ..
make -j
```

This will generate a `bloom-test` binary that contains the tests. The `cmake` command will also
download [googletest](https://www.github.com/google/googletest) on which the tests depend.

There is also a `clang-tidy` target you can use to run [clang-tidy] over the codebase. For this you should pass `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` to the cmake command above. For example:

```sh
mkdir build && cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make -j
make clang-tidy
```

## License

This project is released under the MIT License. For more information, see the LICENSE file.
