/*
 * This code demonstrates how to use NEON SIMD instructions to compute the dot product of two vectors.
 * The code uses the ARM NEON intrinsics to perform the computation.
 * The code is benchmarked using the Google Benchmark library.
 *
 * See ARM Neon Intrinsics Reference: https://arm-software.github.io/acle/neon_intrinsics/advsimd.html
 */

#include <arm_neon.h>
#include <stdio.h>
#include <array>
#include <iostream>
#include <numeric>
#include <benchmark/benchmark.h>

#define ENABLE_SIMD true
#define NO_SIMD false

template <size_t N>
float dot_product_simd(const std::array<float, N> &array1, const std::array<float, N> &array2)
{
    // Represents a vector of 4 single precision float values
    float32x4_t vec = vmovq_n_f32(0.0f);

    for (int i = 0; i < array1.size(); i += 4)
    {
        // Load 4 float values from array1 and array2
        float32x4_t vec1 = vld1q_f32(&array1[i]);
        float32x4_t vec2 = vld1q_f32(&array2[i]);

        float32x4_t result = vmulq_f32(vec1, vec2);
        vec = vaddq_f32(vec, result);
    }

    std::array<float, 4> result;
    vst1q_f32(result.data(), vec);

    float total = std::accumulate(result.begin(), result.end(), 0.0);
    return total;
}

template <size_t IMAGE_SIZE, bool USE_SIMD>
void driver(benchmark::State &state)
{
    std::array<float, IMAGE_SIZE> image1;
    std::array<float, IMAGE_SIZE> image2;

    // Fill the arrays with values 0.0, 0.1, 0.2, etc.
    float value = 0.0f;
    std::generate(image1.begin(), image1.end(), [&value]()
                  {
        float current_value = value;
        value += 0.1f;
        return current_value; });

    value = 0.0f;
    std::generate(image2.begin(), image2.end(), [&value]()
                  {
        float current_value = value;
        value += 0.1f;
        return current_value; });

    for (auto _ : state)
    {
        // Compile-time conditional statement introduced in C++17
        if constexpr (USE_SIMD)
        {
            float ans = dot_product_simd(image1, image2);
            benchmark::DoNotOptimize(ans);
        }
        else
        {
            float ans = std::inner_product(image1.begin(), image1.end(), image2.begin(), 0.0f);
            benchmark::DoNotOptimize(ans);
        }
    }
}

BENCHMARK_TEMPLATE(driver, 32 * 32, ENABLE_SIMD);
BENCHMARK_TEMPLATE(driver, 32 * 32, NO_SIMD);
BENCHMARK_TEMPLATE(driver, 256 * 256, ENABLE_SIMD);
BENCHMARK_TEMPLATE(driver, 256 * 256, NO_SIMD);
BENCHMARK_TEMPLATE(driver, 512 * 512, ENABLE_SIMD);
BENCHMARK_TEMPLATE(driver, 512 * 512, NO_SIMD);

BENCHMARK_MAIN();