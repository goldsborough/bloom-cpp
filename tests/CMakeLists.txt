add_executable(bloom-test test.cpp)
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
