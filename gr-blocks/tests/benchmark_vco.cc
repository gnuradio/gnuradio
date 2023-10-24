/* -*- c++ -*- */
/*
 * Copyright 2002,2004,2005,2013,2018 Free Software Foundation, Inc.
 * Copyright 2023 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "benchmark_common.h"

/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <gnuradio/fxpt_vco.h>
#include <gnuradio/math.h>
#include <spdlog/fmt/fmt.h>
#include <algorithm>
#include <cstdint>
#include <map>
#include <vector>

constexpr uint64_t iterations = 5'000'000;
constexpr uint64_t block_size = 10'000; // fits in cache
constexpr double k = 4.9999999;
constexpr double amplitude = 2.444444444;

void basic_vco(float* output, const float* input)
{
    double phase = 0;

    for (auto j = iterations / block_size; j; --j) {
        for (uint64_t i = 0; i < block_size; i++) {
            output[i] = cos(phase) * amplitude;
            phase += input[i] * k;

            while (phase > 2 * GR_M_PI)
                phase -= 2 * GR_M_PI;

            while (phase < -2 * GR_M_PI)
                phase += 2 * GR_M_PI;
        }
    }
}

template <typename real>
inline real polynomial_approx_sin(real value)
{
    /* The polynomial
     * f(x) = 0.98553 x + 0.14258 x³
     * approximates sin(x) on [-π,+π] to an absolute error below 0.004,
     * f(x) = 0.99999944 x + -0.16665925 x³ + 0.0083165284 x⁵ + -0.0001862801 x^⁷
     * achieves better than 3.5·10⁻⁶ error
     */
    constexpr real a_1 = 0.99999944;
    constexpr real a_3 = -0.16665925;
    constexpr real a_5 = 0.0083165284;
    constexpr real a_7 = -0.0001862801;

    real x_sq = value * value;
    real x_quad = x_sq * x_sq;
    return value * (a_1 + a_3 * x_sq + a_5 * x_quad + a_7 * x_sq * x_quad);
}
void polynomial_vco(float* output, const float* input)
{
    // sin(x) = cos(x + 1.5π) = cos(x - 0.5π)
    for (auto j = iterations / block_size; j; --j) {
        double phase = -GR_M_PI / 2;
        for (uint64_t i = 0; i < block_size; i++) {
            output[i] = polynomial_approx_sin(phase) * amplitude;
            phase += input[i] * k;

            /* we split the handling a bit, because the most common case should be that
             * we're within +-pi; followed by the case where we just positively ran out of
             * that range.
             *
             * The problem with while(too large){make smaller}; while(too small){make
             * larger}; is that at least one float comparison happens without need. The
             * compiler has a hard time noticing that. So, we split into
             *
             * if(too large) { while(too large){make smaller} }
             * else {while(too small) {embiggen}}
             *
             * And because there's then still one initial redundant comparison, we go for
             *
             * if(too large) { do{make smaller} while(still to large); }
             * else {while(too small) {embiggen}}
             *
             */
            if (phase > GR_M_PI) {
                do {
                    phase -= 2 * GR_M_PI;
                } while (phase > GR_M_PI);
            } else {
                while (phase < GR_M_PI) {
                    phase += 2 * GR_M_PI;
                }
            }
        }
    }
}

void fxpt_vco(float* output, const float* input)
{
    gr::fxpt_vco vco;

    for (auto j = iterations / block_size; j; --j) {
        vco.cos(output, input, block_size, k, amplitude);
    }
}

// ----------------------------------------------------------------

void nop_fct(float* x, const float* y) {}

void nop_loop(float* x, const float* y)
{
    for (auto i = iterations; i; --i) {
        nop_fct(x, y);
    }
}

int main(int argc, char** argv)
{
    auto bench = [](decltype(basic_vco) func) { return benchmark(func, block_size); };
    using duration_t = decltype(bench(nop_loop));

    std::map<std::string, decltype(&basic_vco)> funcs{
        //{ "nop loop", nop_loop },
        { "basic VCO", basic_vco },
        { "polynomial VCO", polynomial_vco },
        { "fxpt VCO", fxpt_vco }
    };

    std::vector<std::pair<duration_t, std::string>> times;
    for (auto& [name, func] : funcs) {
        times.emplace_back(bench(func), name);
    }

    std::sort(times.begin(), times.end());
    std::vector<std::string> lines;
    size_t maxlen = 0;
    for (const auto& [time, name] : times) {
        lines.emplace_back(format_duration(name, time, iterations, block_size));
        maxlen = std::max(lines.back().size(), maxlen);
    }
    fmt::print("+{1:—^{0}}+\n", maxlen + 2, "");
    for (const auto& line : lines) {
        fmt::print("|{1:^{0}}|\n", maxlen + 2, line);
    }
    fmt::print("+{1:—^{0}}+\n", maxlen + 2, "");
}
