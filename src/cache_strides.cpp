#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <cstdlib>

std::vector<unsigned int> parse_stride_sizes_from_env()
{
    std::vector<unsigned int> stride_sizes;
    const char *env_str = std::getenv("STRIDE_SIZES");
    if (env_str)
    {
        std::istringstream env_stream(env_str);
        std::string stride_size_str;
        while (env_stream >> stride_size_str)
        {
            unsigned int stride_size = std::stoi(stride_size_str);
            if (stride_size > 0)
            {
                stride_sizes.push_back(stride_size);
            }
        }
    }
    return stride_sizes;
}

int main(int argc, char *argv[])
{
    std::vector<unsigned int> stride_sizes;

    // First, check if stride sizes are passed as command-line arguments
    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            unsigned int stride_size = std::stoi(argv[i]);
            if (stride_size <= 0)
            {
                std::cerr << "Stride size must be a positive number" << std::endl;
                return 1;
            }
            stride_sizes.push_back(stride_size);
        }
    }
    else
    {
        // If no arguments are passed, check the environment variable STRIDE_SIZES
        stride_sizes = parse_stride_sizes_from_env();
        if (stride_sizes.empty())
        {
            std::cerr << "No stride sizes provided. Either pass them as arguments or set the STRIDE_SIZES environment variable." << std::endl;
            return 1;
        }
    }

    constexpr unsigned int CACHE_LINE_SIZE = 128;
    constexpr unsigned int NUM_STRIDES = 1000000;
    constexpr unsigned int NUM_ITER = 100;

    for (unsigned int stride_size : stride_sizes)
    {
        std::vector<long long> iteration_times;

        for (unsigned int iter = 0; iter < NUM_ITER; ++iter)
        {
            std::vector<char> arr(CACHE_LINE_SIZE * NUM_STRIDES, 0);
            auto start = std::chrono::high_resolution_clock::now();

            for (unsigned int pos = 0; pos < arr.size(); pos += stride_size)
            {
                arr[pos] += 1;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            iteration_times.push_back(duration);
        }

        // Calculate min, median, and max
        auto min_time = *std::min_element(iteration_times.begin(), iteration_times.end());
        auto max_time = *std::max_element(iteration_times.begin(), iteration_times.end());

        // Sorting to find the median
        std::sort(iteration_times.begin(), iteration_times.end());
        auto median_time = iteration_times[iteration_times.size() / 2];

        std::cout << "Timing results (in microseconds) for stride size " << stride_size << " bytes:" << std::endl;
        std::cout << "Min time: " << min_time << " us" << std::endl;
        std::cout << "Median time: " << median_time << " us" << std::endl;
        std::cout << "Max time: " << max_time << " us" << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
