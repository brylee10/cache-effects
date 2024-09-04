# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(CacheEffects LANGUAGES CXX ASM)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Output executables to the 'bin' directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Enable debug info to inspect the assembly (note that the build will still be optimized)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")

include(FetchContent)
# Disable the Google Benchmark requirement on Google Test
set(BENCHMARK_ENABLE_TESTING NO)

# Fetch Google benchmark from its GitHub repository
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG origin/main
)

FetchContent_MakeAvailable(googlebenchmark)

# Benchmarks
add_executable(single_var_cache_coherence src/single_var_cache_coherence.cpp)
target_link_libraries(single_var_cache_coherence benchmark::benchmark)

add_executable(false_sharing_bench src/false_sharing.cpp)
target_link_libraries(false_sharing_bench benchmark::benchmark)

add_executable(cache_strides_bench src/cache_strides.cpp)

add_executable(cache_misses src/cache_misses.cpp)

add_executable(simd_dot src/simd_dot.cpp)
target_link_libraries(simd_dot benchmark::benchmark)

# Considered copying the assembly output from cmake via https://gist.github.com/Manu343726/64c0a75c089ad96d22cb
# but the assembly does not contain debug info.
# Use `objump` to dump the desired line numbers and source code (-S shows inlined code with assembly, -l shows line numbers)
add_custom_command(TARGET cache_strides_bench
    POST_BUILD
    COMMAND objdump -l -S
        "${CMAKE_BINARY_DIR}/CMakeFiles/cache_strides_bench.dir/src/cache_strides.cpp.o" > 
        "asm/cache_strides.cpp.s"
    COMMAND objdump -l -S
        "${CMAKE_BINARY_DIR}/CMakeFiles/cache_misses.dir/src/cache_misses.cpp.o" > 
        "asm/cache_misses.cpp.s"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

