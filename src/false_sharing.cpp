/**
 * @file false_sharing.cpp
 * @brief A program to benchmark the impact of false sharing in a multithreaded environment.
 *
 * This benchmark demonstrates the concept of false sharing by spawning multiple threads that
 * access and update separate `std::atomic<int>` variables. False sharing occurs when multiple
 * threads modify independent variables that reside on the same cache line, leading to unnecessary
 * cache invalidations and performance degradation.
 *
 */
#include <benchmark/benchmark.h>
#include <spdlog/spdlog.h>
#include <atomic>
#include <thread>
#include <vector>

constexpr int MAX_THREADS = 8;
constexpr int CACHE_LINE_SIZE_BYTES = 128; // Adjusted for Mac M1 cache line size

struct alignas(CACHE_LINE_SIZE_BYTES) AlignedAtomic
{
    std::atomic<int> value{0};
};

struct UnalignedAtomic
{
    std::atomic<int> value{0};
};

template <typename AtomicType, unsigned int num_threads, unsigned int iterations>
void false_sharing_benchmark(benchmark::State &state)
{
    spdlog::set_level(spdlog::level::err);
    static_assert(num_threads >= 1 && num_threads <= MAX_THREADS, "Invalid number of threads");

    std::vector<std::thread> threads(num_threads);
    std::vector<AtomicType> messages(num_threads);

    for (auto _ : state)
    {
        for (int i = 0; i < num_threads; ++i)
        {
            threads[i] = std::thread([&messages, i]()
                                     {
                for (int j = 0; j < iterations; ++j) {
                    messages[i].value.fetch_add(1, std::memory_order_relaxed);
                } });
        }

        for (auto &t : threads)
        {
            t.join();
        }
    }
}

BENCHMARK_TEMPLATE(false_sharing_benchmark, AlignedAtomic, 1, 10000);
BENCHMARK_TEMPLATE(false_sharing_benchmark, AlignedAtomic, 2, 10000);
BENCHMARK_TEMPLATE(false_sharing_benchmark, AlignedAtomic, 3, 10000);
BENCHMARK_TEMPLATE(false_sharing_benchmark, AlignedAtomic, 5, 10000);

BENCHMARK_TEMPLATE(false_sharing_benchmark, UnalignedAtomic, 1, 10000);
BENCHMARK_TEMPLATE(false_sharing_benchmark, UnalignedAtomic, 2, 10000);
BENCHMARK_TEMPLATE(false_sharing_benchmark, UnalignedAtomic, 3, 10000);
BENCHMARK_TEMPLATE(false_sharing_benchmark, UnalignedAtomic, 5, 10000);

BENCHMARK_MAIN();