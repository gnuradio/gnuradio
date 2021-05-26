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

#include "ad9081_sink_impl.h"
#include "device_source_impl.h"

#include <gnuradio/blocks/complex_to_float.h>
#include <gnuradio/blocks/float_to_short.h>
#include <gnuradio/io_signature.h>

#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

namespace gr {
namespace iio {

ad9081_sink_f32c::ad9081_sink_f32c(const std::array<bool, MAX_CHANNEL_COUNT>& en,
                                   ad9081_sink::sptr src_block)
    : hier_block2("ad9081_sink_f32c",
                  gr::io_signature::make(
                      get_channel_count(en), get_channel_count(en), sizeof(gr_complex)),
                  gr::io_signature::make(0, 0, 0)),
      d_ad9081_block(src_block)
{
    basic_block_sptr hier = self();
    int n = get_channel_count(en);

    for (int i = 0; i < n; i++) {
        auto f2s1 = gr::blocks::float_to_short::make(1, 8192.0);
        auto f2s2 = gr::blocks::float_to_short::make(1, 8192.0);
        auto c2f = gr::blocks::complex_to_float::make(1);

        connect(hier, i, c2f, 0);
        connect(c2f, 0, f2s1, 0);
        connect(c2f, 1, f2s2, 0);
        connect(f2s1, 0, d_ad9081_block, 2 * i);
        connect(f2s2, 0, d_ad9081_block, 2 * i + 1);
    }
}

ad9081_sink_f32c::sptr ad9081_sink_f32c::make(const std::string& uri,
                                              std::array<bool, MAX_CHANNEL_COUNT> en,
                                              size_t buffer_size,
                                              bool cyclic)
{
    ad9081_sink::sptr block = ad9081_sink::make(uri, en, buffer_size, cyclic);

    return gnuradio::get_initial_sptr(new ad9081_sink_f32c(en, block));
}

ad9081_sink::sptr ad9081_sink::make(const std::string& uri,
                                    std::array<bool, MAX_CHANNEL_COUNT> en,
                                    size_t buffer_size,
                                    bool cyclic)

{
    return ad9081_sink::make_from(
        device_source_impl::get_context(uri), en, buffer_size, cyclic);
}

ad9081_sink::sptr ad9081_sink::make_from(struct iio_context* ctx,
                                         std::array<bool, MAX_CHANNEL_COUNT> en,
                                         size_t buffer_size,
                                         bool cyclic)
{
    return gnuradio::get_initial_sptr(new ad9081_sink_impl(ctx, en, buffer_size, cyclic));
}

ad9081_sink_impl::ad9081_sink_impl(struct iio_context* ctx,
                                   std::array<bool, MAX_CHANNEL_COUNT> en,
                                   size_t buffer_size,
                                   bool cyclic)
    : gr::sync_block("ad9081_sink",
                     gr::io_signature::make(1, -1, sizeof(int16_t)),
                     gr::io_signature::make(0, 0, 0)),
      device_sink_impl(ctx,
                       destroy_ctx,
                       "axi-ad9081-tx-hpc",
                       get_channel_vector(en),
                       "axi-ad9081-rx-hpc",
                       std::vector<std::string>(),
                       buffer_size,
                       0,
                       cyclic)
{
    for (int i = 0; i < MAX_CHANNEL_COUNT; i++) {
        auto& st = d_channel_state[i];
        st.enabled = en[i];

        if (!st.enabled)
            continue;

        d_last_active_channel = i;

        std::string ch_i_name = "voltage" + std::to_string(i) + "_i";
        std::string ch_q_name = "voltage" + std::to_string(i) + "_q";

        st.ch_i = iio_device_find_channel(phy, ch_i_name.c_str(), true);
        st.ch_q = iio_device_find_channel(phy, ch_q_name.c_str(), true);

        if (!st.ch_i || !st.ch_q)
            throw std::runtime_error("ad9081: Failed to acquire channel voltage" +
                                     std::to_string(i) + "_[iq]!");

        long long int converter_rate;
        auto ret =
            iio_channel_attr_read_longlong(st.ch_i, "dac_frequency", &converter_rate);
        if (ret)
            throw std::runtime_error(
                "ad9081: Failed to read dac_frequency from iio channel!");

        st.converter_rate = static_cast<int64_t>(converter_rate);

        parse_datapath(st);

        d_channel_ncos[st.channel_nco] = (int8_t)i;
        d_main_ncos[st.main_nco] = (int8_t)i;
    }

    if (d_last_active_channel == -1)
        throw std::runtime_error("ad9081: No channels active!");
}

} /* namespace iio */
} /* namespace gr */
