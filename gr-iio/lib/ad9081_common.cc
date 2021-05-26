/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ad9081_common.h"

#include <cmath>
#include <cstdint>

namespace gr {
namespace iio {

void parse_datapath(ad9081_channel_state& st)
{
    char label[20];

    int ret = iio_channel_attr_read(st.ch_i, "label", label, sizeof(label));
    if (ret < 0)
        throw std::runtime_error("ad9081: Failed to read label from channel!");

    // FDDC0->CDDC0->ADC0
    // FDUC0->CDUC0->DAC0
    // 0   4      11    17

    int value = label[4] - '0';
    if (!(0 <= value && value < ad9081_common::N_FNCO))
        throw std::runtime_error("ad9081: Label parse failure: Invalid channel nco: " +
                                 std::to_string(value));
    st.channel_nco = value;

    value = label[11] - '0';
    if (!(0 <= value && value < ad9081_common::N_CNCO))
        throw std::runtime_error("ad9081: Label parse failure: Invalid main nco: " +
                                 std::to_string(value));
    st.main_nco = value;

    value = label[17] - '0';
    if (!(0 <= value && value < ad9081_common::N_CNCO))
        throw std::runtime_error("ad9081: Label parse failure: Invalid adc: " +
                                 std::to_string(value));
    st.converter = value;
}

void ad9081_common::set_main_nco_freq(int nco, double freq)
{
    set_nco_attr<N_CNCO>("main_nco_frequency", d_main_ncos, nco, std::lround(freq));
}

void ad9081_common::set_main_nco_phase(int nco, float phase)
{
    set_nco_attr<N_CNCO>(
        "main_nco_phase", d_main_ncos, nco, phase_fold(180.0f / M_PIf32 * phase));
}

void ad9081_common::set_channel_nco_freq(int nco, double freq)
{
    set_nco_attr<N_FNCO>("channel_nco_frequency", d_channel_ncos, nco, std::lround(freq));
}

void ad9081_common::set_channel_nco_phase(int nco, float phase)
{
    set_nco_attr<N_FNCO>(
        "channel_nco_phase", d_channel_ncos, nco, phase_fold(180.0f / M_PIf32 * phase));
}


} /* namespace iio */
} /* namespace gr */
