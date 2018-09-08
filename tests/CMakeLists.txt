set(BLOOM_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)

add_executable(bloom-test ${BLOOM_TEST_SOURCES})
add_dependencies(bloom-test gtest)

target_link_libraries(bloom-test PRIVATE bloom gtest_main)
target_compile_options(bloom-test PRIVATE
  -Wall
  -Wextra
  -pedantic
)

set_property(TARGET bloom-test PROPERTY CXX_STANDARD 17)
set_property(TARGET bloom-test PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET bloom-test PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_test(NAME test COMMAND bloom-test)

if (NOT CLANG_TIDY_BINARY)
  set(CLANG_TIDY_BINARY clang-tidy)
endif()
message(STATUS ${CLANG_TIDY_BINARY})
add_custom_target(clang-tidy
  COMMAND ${CLANG_TIDY_BINARY} -p ${CMAKE_BINARY_DIR} ${BLOOM_TEST_SOURCES})
