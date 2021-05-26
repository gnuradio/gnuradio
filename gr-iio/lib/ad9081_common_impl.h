/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_AD9081_COMMON_IMPL_H
#define INCLUDED_IIO_AD9081_COMMON_IMPL_H

#include <gnuradio/iio/ad9081_common.h>

#include <array>
#include <cstdint>
#include <cstdlib>
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

class ad9081_common_impl : virtual public ad9081_common
{
protected:
    std::array<ad9081_channel_state, MAX_CHANNEL_COUNT> d_channel_state;
    std::array<int8_t, 8> d_channel_ncos = { -1, -1, -1, -1, -1, -1, -1, -1 };
    std::array<int8_t, 4> d_main_ncos = { -1, -1, -1, -1 };

    int d_last_active_channel = -1;

    template <int N>
    void
    set_nco_attr(const char* attr, std::array<int8_t, N>& src, int nco, long long val)
    {
        if (!(0 <= nco && nco <= N - 1))
            throw std::runtime_error("ad9081: set_nco_attr: NCO id out of bounds: " +
                                     std::to_string(nco));

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
    void set_main_nco_freq(int nco, int64_t freq) override;

    void set_main_nco_phase(int nco, float phase) override;

    void set_channel_nco_freq(int nco, int64_t freq) override;

    void set_channel_nco_phase(int nco, float phase) override;
};

long long phase_fold(float phase);

void parse_datapath(ad9081_channel_state& st);

template <size_t N>
inline int get_channel_count(const std::array<bool, N>& en)
{
    int count = 0;

    for (auto i = en.cbegin(); i != en.cend(); i++)
        count += (int)*i;

    return count;
}

namespace {

template <size_t N>
std::vector<std::string> get_channel_vector(const std::array<bool, N>& chs)
{
    std::vector<std::string> channels;

    for (size_t i = 0; i < N; i++) {
        if (chs[i]) {
            channels.push_back("voltage" + std::to_string(i) + "_i");
            channels.push_back("voltage" + std::to_string(i) + "_q");
        }
    }

    return channels;
}

} // namespace

} // namespace iio
} // namespace gr

#endif // INCLUDED_IIO_AD9081_COMMON_IMPL_H
