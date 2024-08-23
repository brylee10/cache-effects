# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(CacheEffects LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Output executables to the 'bin' directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
# Disable the Google Benchmark requirement on Google Test
set(BENCHMARK_ENABLE_TESTING NO)

# Fetch Google benchmark from its GitHub repository
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG origin/main
)

# Make the spdlog content available
FetchContent_MakeAvailable(googlebenchmark)

# Fetch spdlog from its GitHub repository
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.0 
)

# Make the spdlog content available
FetchContent_MakeAvailable(spdlog)

# Benchmarks
add_executable(false_sharing_bench src/false_sharing.cpp)
target_link_libraries(false_sharing_bench benchmark::benchmark spdlog::spdlog)
