/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_AD9081_COMMON_IMPL_H
#define INCLUDED_IIO_AD9081_COMMON_IMPL_H

#include <array>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <iio.h>

namespace gr {
namespace iio {

struct ad9081_channel_state {
    bool enabled;
    struct iio_channel* ch_i;
    struct iio_channel* ch_q;
    int64_t converter_rate;
    uint8_t channel_nco;
    uint8_t main_nco;
    uint8_t converter;

    inline ad9081_channel_state() = default;

    inline ad9081_channel_state(bool enabled,
                                struct iio_channel* ch_i,
                                struct iio_channel* ch_q,
                                int64_t converter_rate)
        : enabled(enabled), ch_i(ch_i), ch_q(ch_q), converter_rate(converter_rate)
    {
    }
};

class ad9081_common
{
public:
    static constexpr int MAX_CHANNEL_COUNT = 8;
    static constexpr int N_FNCO = 8;
    static constexpr int N_CNCO = 4;

protected:
    std::array<ad9081_channel_state, MAX_CHANNEL_COUNT> d_channel_state;
    std::array<int8_t, N_FNCO> d_channel_ncos{ -1, -1, -1, -1, -1, -1, -1, -1 };
    std::array<int8_t, N_CNCO> d_main_ncos{ -1, -1, -1, -1 };

    int d_last_active_channel = -1;

    static constexpr size_t INTERNAL_BUFFER_SIZE = 1024;
    std::array<short, INTERNAL_BUFFER_SIZE> s_buffer_i;
    std::array<short, INTERNAL_BUFFER_SIZE> s_buffer_q;
    std::array<float, INTERNAL_BUFFER_SIZE> f_buffer_i;
    std::array<float, INTERNAL_BUFFER_SIZE> f_buffer_q;

    template <int N>
    void set_nco_attr(const char* attr, std::array<int8_t, N>& src, int nco, int64_t val)
    {
        if (!(0 <= nco && nco < N))
            throw std::runtime_error("ad9081: set_nco_attr: NCO id out of bounds: " +
                                     std::to_string(nco) + ", N: " + std::to_string(N));

        int ch_id = src[nco];
        if (ch_id == -1)
            return; // Unused NCO

        auto& st = d_channel_state[ch_id];
        int ret = iio_channel_attr_write_longlong(st.ch_i, attr, val);
        if (ret)
            throw std::runtime_error(
                "ad9081: set_nco_attr: Failed to write NCO attribute: " +
                std::string(attr));
    }

public:
    void set_main_nco_freq(int nco, double freq);

    void set_main_nco_phase(int nco, float phase);

    void set_channel_nco_freq(int nco, double freq);

    void set_channel_nco_phase(int nco, float phase);
};

inline constexpr int64_t phase_fold(float x)
{
    int64_t n = static_cast<int64_t>(std::floor((x + 180.0f) / 360.0f));
    return (int64_t)(1000LL * (x - n * 360.0f));
}

void parse_datapath(ad9081_channel_state& st);

template <size_t N>
inline int get_channel_count(const std::array<bool, N>& en)
{
    int count = 0;

    for (auto enabled : en)
        count += enabled;

    return count;
}

namespace {

template <size_t N>
std::vector<std::string> get_channel_vector(const std::array<bool, N>& chs)
{
    std::vector<std::string> channels;

    for (size_t i = 0; i < N; i++) {
        if (chs[i]) {
            channels.emplace_back("voltage" + std::to_string(i) + "_i");
            channels.emplace_back("voltage" + std::to_string(i) + "_q");
        }
    }

    return channels;
}

} // namespace

} // namespace iio
} // namespace gr

#endif // INCLUDED_IIO_AD9081_COMMON_IMPL_H
