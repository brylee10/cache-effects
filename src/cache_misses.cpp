/*
 * This program is used to measure the number of cache misses that occur when accessing memory with different stride sizes.
 * Uses MacOS specific XCode Instruments APIs to emit signposts for measuring cache misses.
 */

#include <iostream>
#include <atomic>
#include <os/log.h>
#include <os/signpost.h>
#include <vector>

constexpr unsigned int MAX_STRIDE_SIZE = 256;
constexpr unsigned int NUM_ITERATIONS = 10000;
constexpr unsigned int ARR_SIZE = MAX_STRIDE_SIZE * NUM_ITERATIONS;

// Allow default initialization of the atomic variable to 0.
// Reduce compiler optimizations (e.g. the read from the fetch add in the increment op)
// by using volatile keyword
struct VolatileAtomicChar
{
    volatile std::atomic<char> value;

    VolatileAtomicChar() : value(0) {}
};

void add(std::vector<VolatileAtomicChar> &vec, unsigned int stride_size)
{
    int pos = 0;
    for (int it = 0; it < NUM_ITERATIONS;)
    {
        vec[pos].value += 1;
        it++;
        pos += stride_size;
    }
}

int main()
{
    // Original Hypothesis: let N be the number of cache misses at the 128 byte stride mark
    // a stride S >= 128 bytes will have N cache misses
    // a stride S < 128 bytes will have S / 128 * N cache misses
    std::vector<unsigned int> stride_sizes = {1, 4, 16, 64, 128, 256};
    os_log_t log = os_log_create("Cache Misses", "PointsOfInterest");

    // MacOS specific logging API which emits signposts for indicating a point of interest in XCode Instruments.
    // See the developer docs: https://developer.apple.com/documentation/os/logging/os_log_t
    os_signpost_event_emit(log, OS_SIGNPOST_ID_EXCLUSIVE, "Starting");

    for (auto stride_size : stride_sizes)
    {
        std::vector<VolatileAtomicChar> arr(ARR_SIZE);
        os_signpost_event_emit(log, OS_SIGNPOST_ID_EXCLUSIVE, "Starting Stride");
        add(arr, stride_size);
        os_signpost_event_emit(log, OS_SIGNPOST_ID_EXCLUSIVE, "Finished Stride");
    }

    os_signpost_event_emit(log, OS_SIGNPOST_ID_EXCLUSIVE, "Ending");
    return 0;
}