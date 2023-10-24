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

/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <gnuradio/fxpt_nco.h>
#include <gnuradio/math.h>
#include <gnuradio/nco.h>
#include <spdlog/fmt/fmt.h>
#include <string_view>
#include <algorithm>
#include <cstdint>
#include <map>
#include <vector>

constexpr uint64_t iterations = 20000000;
constexpr uint64_t block_size = 10'000; // fits in cache
constexpr double freq = 5003.123;
#include "benchmark_common.h"

// ----------------------------------------------------------------
// Don't compare the _vec with other functions since memory store's
// are involved.

void basic_sincos_vec(float* x, float* y)
{
    gr::nco<float, float> nco;

    nco.set_freq(2 * GR_M_PI / freq);

    for (auto i = iterations / block_size; i; --i) {
        for (uint64_t j = 0; j < block_size; j++) {
            nco.sincos(&x[2 * j + 1], &x[2 * j]);
            nco.step();
        }
    }
}

void native_sincos_vec(float* x, float* y)
{
    gr::nco<float, float> nco;

    nco.set_freq(2 * GR_M_PI / freq);

    for (auto i = iterations / block_size; i; --i) {
        nco.sincos((gr_complex*)x, block_size);
    }
}

void fxpt_sincos_vec(float* x, float* y)
{
    gr::fxpt_nco nco;

    nco.set_freq(2 * GR_M_PI / freq);

    for (auto i = iterations / block_size; i; --i) {
        nco.sincos((gr_complex*)x, block_size);
    }
}

// ----------------------------------------------------------------

void native_sincos(float* x, float* y)
{
    gr::nco<float, float> nco;

    nco.set_freq(2 * GR_M_PI / freq);

    for (auto i = iterations; i; --i) {
        nco.sincos(x, y);
        nco.step();
    }
}

void fxpt_sincos(float* x, float* y)
{
    gr::fxpt_nco nco;

    nco.set_freq(2 * GR_M_PI / freq);

    for (auto i = iterations; i; --i) {
        nco.sincos(x, y);
        nco.step();
    }
}

// ----------------------------------------------------------------

void native_sin(float* x, float* y)
{
    gr::nco<float, float> nco;

    nco.set_freq(2 * GR_M_PI / freq);

    for (auto i = iterations; i; --i) {
        *x = nco.sin();
        nco.step();
    }
}

void fxpt_sin(float* x, float* y)
{
    gr::fxpt_nco nco;

    nco.set_freq(2 * GR_M_PI / freq);

    for (auto i = iterations; i; --i) {
        *x = nco.sin();
        nco.step();
    }
}

// ----------------------------------------------------------------

void nop_fct(float* x, float* y) {}

void nop_loop(float* x, float* y)
{
    for (auto i = iterations; i; --i) {
        nop_fct(x, y);
    }
}

int main(int argc, char** argv)
{
    auto bench = [](decltype(nop_loop) func) { return benchmark(func, block_size); };
    using duration_t = decltype(bench(nop_loop));

    std::map<std::string, decltype(&nop_loop)> funcs{
        // { "nop loop", nop_loop },
        { "native sine", native_sin },
        { "fxpt sine", fxpt_sin },
        { "native sin/cos", native_sincos },
        { "fxpt sin/cos", fxpt_sincos },
        { "basic sin/cos vec", basic_sincos_vec },
        { "native sin/cos vec", native_sincos_vec },
        { "fxpt sin/cos vec", fxpt_sincos_vec }
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
