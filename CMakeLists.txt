cmake_minimum_required(VERSION 3.2)
project(bloom)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BLOOM_WITH_TESTS "Build tests" OFF)

add_library(bloom INTERFACE)

set(BLOOM_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/bloom/bloom.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/bloom/hash.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/bloom/slice.hpp
)

target_sources(bloom INTERFACE $<BUILD_INTERFACE:${BLOOM_HEADERS}>)
target_include_directories(bloom INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(bloom SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

if(${BLOOM_WITH_TESTS} OR (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    enable_testing()
    include(cmake/googletest/googletest.cmake)
    add_subdirectory(tests)
endif()
