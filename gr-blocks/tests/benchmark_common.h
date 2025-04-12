/* -*- c++ -*- */
/*
 * Copyright 2002,2004,2013,2018 Free Software Foundation, Inc.
 * Copyright 2023 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_BENCHMARK_COMMON
#define INCLUDED_BENCHMARK_COMMON
/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <gnuradio/random.h>
#include <spdlog/fmt/fmt.h>
#include <string_view>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <limits>
#include <numeric>
#include <vector>

template <typename functor>
[[nodiscard]] auto benchmark(functor test, size_t block_size)
{
    std::vector<float> outp(2 * block_size);
    float* output = outp.data();
    float *x = &output[0], *y = &output[block_size];

    // generate input in the first half, and also in the second half to touch the memory
    gr::xoroshiro128p_prng rng(42);
    for (auto& value : outp) {
        value = rng() / static_cast<double>(1ULL << 32) - (1ULL << 32);
    }

    auto before = std::chrono::high_resolution_clock::now();
    // do the actual work

    test(x, y);

    auto after = std::chrono::high_resolution_clock::now();
    // get ending CPU usage
    auto dur =
        std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(
            after - before);

    // prevent the compiler from discarding the output, not doing the calculations.
    volatile auto sum = std::accumulate(outp.cbegin(), outp.cend(), 0.0f);
    if (sum == std::numeric_limits<decltype(sum)>::min()) {
        // should never be hit
        return decltype(dur){};
    }

    return dur;
}
template <typename dur_t>
auto format_duration(std::string_view name,
                     dur_t dur,
                     size_t iterations,
                     size_t block_size)
{
    auto dur_s = std::chrono::duration_cast<std::chrono::duration<double>>(dur);
    return fmt::format(FMT_STRING("{:<18} time: {:<8.4e} s throughput: {:>6.3e} it/s"),
                       name,
                       dur_s.count(),
                       static_cast<double>(iterations) / dur_s.count());
}

#endif
