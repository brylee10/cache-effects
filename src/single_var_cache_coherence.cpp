/**
 * Benchmarks measuring the impact of atomic operations with and without cache contention and flushing.
 */

#include <benchmark/benchmark.h>
#include <atomic>
#include <thread>

unsigned int ITERATIONS = 100000;

// A non-atomic load operation with no contention.
void control_benchmark(benchmark::State &state)
{
    volatile int x = 0;
    unsigned int num_threads = 1;
    std::vector<std::thread> threads(num_threads);

    for (auto _ : state)
    {
        threads[0] = std::thread([&x]()
                                 {
                int accum = 0;
                for (int j = 0; j < ITERATIONS; ++j) {
                    accum += x;
                } assert(accum == 0); });

        for (auto &t : threads)
        {
            t.join();
        }
    }
}

// Tests multiple loads against an atomic read only variable.
void coherence_no_contention_benchmark(benchmark::State &state)
{
    volatile std::atomic<int> value{0};
    int x = 0;
    unsigned int num_threads = 2;
    std::vector<std::thread> threads(num_threads);

    for (auto _ : state)
    {
        threads[0] = std::thread([&value]()
                                 {
                for (int j = 0; j < ITERATIONS; ++j) {
                    value.load(std::memory_order_relaxed);
                } });

        threads[1] = std::thread([&value, &x]()
                                 {
                int accum = 0;
                for (int j = 0; j < ITERATIONS; ++j) {
                    int x = value.load(std::memory_order_relaxed);
                    accum += x;
                } assert(accum == 0); });

        for (auto &t : threads)
        {
            t.join();
        }
    }
}

// An atomic load operation with contention from a store on the same variable.
void coherence_contention_benchmark(benchmark::State &state)
{
    volatile std::atomic<int> value{0};

    unsigned int num_threads = 2;
    unsigned int iterations = 10000;
    std::vector<std::thread> threads(num_threads);

    for (auto _ : state)
    {
        threads[0] = std::thread([&value]()
                                 {
                for (int j = 0; j < ITERATIONS; ++j) {
                    value.store(1, std::memory_order_relaxed);
                } });

        threads[1] = std::thread([&value]()
                                 {
                int accum = 0;
                for (int j = 0; j < ITERATIONS; ++j) {
                    int x = value.load(std::memory_order_relaxed);
                    accum += x;
                } 
                assert(accum > 0); });

        for (auto &t : threads)
        {
            t.join();
        }
    }
}

BENCHMARK(control_benchmark);
BENCHMARK(coherence_no_contention_benchmark);
BENCHMARK(coherence_contention_benchmark);

BENCHMARK_MAIN();