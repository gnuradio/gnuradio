/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ad9081_common_impl.h"

#include <gnuradio/iio/ad9081_common.h>

#include <cmath>
#include <cstdint>

namespace gr {
namespace iio {

long long phase_fold(float x)
{
    return (long long)(1000 *
                       (x - static_cast<int64_t>(floor((x + 180.0) / 360.0)) * 360.0));
}

int64_t nyquist_fold(int64_t f_s, int64_t f_c /*, int64_t& f_lo, bool& odd*/)
{
    int64_t f_n = f_s >> 1;

    bool neg = f_c < 0;
    if (neg)
        f_c = -f_c;

    int64_t n = (f_c / f_n + 1) >> 1;

    int r = f_c - n * f_s;
    if (neg)
        r = -r;

    // odd = !(n & 1);
    // f_lo = r;
    return r;
}

void parse_datapath(ad9081_channel_state& st)
{
    int ret;
    char label[20];
    int v;

    ret = iio_channel_attr_read(st.ch_i, "label", label, sizeof(label));
    if (ret < 0)
        throw std::runtime_error("ad9081: Failed to read label from channel!");

    // FDDC0->CDDC0->ADC0
    // FDUC0->CDUC0->DAC0
    // 0   4      11    17

    v = label[4] - '0';
    if (!(0 <= v && v < 8))
        throw std::runtime_error("ad9081: Label parse failure: Invalid channel nco: " +
                                 std::to_string(v));
    st.channel_nco = v;

    v = label[11] - '0';
    if (!(0 <= v && v < 4))
        throw std::runtime_error("ad9081: Label parse failure: Invalid main nco: " +
                                 std::to_string(v));
    st.main_nco = v;

    v = label[17] - '0';
    if (!(0 <= v && v < 4))
        throw std::runtime_error("ad9081: Label parse failure: Invalid adc: " +
                                 std::to_string(v));
    st.converter = v;
}

void ad9081_common_impl::set_main_nco_freq(int nco, int64_t freq)
{
    set_nco_attr<4>("main_nco_frequency", d_main_ncos, nco, freq);
}

void ad9081_common_impl::set_main_nco_phase(int nco, float phase)
{
    set_nco_attr<4>("main_nco_phase", d_main_ncos, nco, phase_fold(180.0 / M_PI * phase));
}

void ad9081_common_impl::set_channel_nco_freq(int nco, int64_t freq)
{
    set_nco_attr<8>("channel_nco_frequency", d_channel_ncos, nco, freq);
}

void ad9081_common_impl::set_channel_nco_phase(int nco, float phase)
{
    set_nco_attr<8>(
        "channel_nco_phase", d_channel_ncos, nco, phase_fold(180.0 / M_PI * phase));
}


} /* namespace iio */
} /* namespace gr */
