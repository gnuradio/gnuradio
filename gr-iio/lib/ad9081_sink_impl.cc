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

#include "ad9081_sink_impl.h"
#include "device_source_impl.h"

#include <gnuradio/io_signature.h>
#include <volk/volk.h>

#include <array>
#include <fstream>
#include <string>
#include <vector>

namespace gr {
namespace iio {

ad9081_sink::sptr ad9081_sink::make(const std::string& uri,
                                    const std::array<bool, MAX_CHANNEL_COUNT>& en,
                                    size_t buffer_size,
                                    bool cyclic)
{
    return gnuradio::make_block_sptr<ad9081_sink_impl>(
        device_source_impl::get_context(uri), en, buffer_size, cyclic);
}

ad9081_sink_impl::ad9081_sink_impl(struct iio_context* ctx,
                                   const std::array<bool, MAX_CHANNEL_COUNT>& en,
                                   size_t buffer_size,
                                   bool cyclic)
    : gr::sync_block("ad9081_sink",
                     gr::io_signature::make(1, -1, sizeof(gr_complex)),
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

        long long converter_rate;
        auto ret =
            iio_channel_attr_read_longlong(st.ch_i, "dac_frequency", &converter_rate);
        if (ret)
            throw std::runtime_error(
                "ad9081: Failed to read dac_frequency from iio channel!");

        st.converter_rate = static_cast<int64_t>(converter_rate);

        parse_datapath(st);

        d_channel_ncos[st.channel_nco] = static_cast<int8_t>(i);
        d_main_ncos[st.main_nco] = static_cast<int8_t>(i);
    }

    if (d_last_active_channel == -1)
        throw std::runtime_error("ad9081: No channels active!");
}

void ad9081_sink_impl::channel_write(const struct iio_channel* chn,
                                     const void* src,
                                     size_t offset,
                                     size_t len)
{
    uintptr_t dst_ptr, src_ptr = (uintptr_t)src, end = src_ptr + len;
    unsigned int length = iio_channel_get_data_format(chn)->length / 8;
    uintptr_t buf_end = (uintptr_t)iio_buffer_end(buf);
    ptrdiff_t buf_step = iio_buffer_step(buf) * (interpolation + 1);

    for (dst_ptr = (uintptr_t)iio_buffer_first(buf, chn) + buf_step * offset;
         dst_ptr < buf_end && src_ptr + length <= end;
         dst_ptr += buf_step, src_ptr += length)
        iio_channel_convert_inverse(chn, (void*)dst_ptr, (const void*)src_ptr);
}

int ad9081_sink_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    int ret;

    if (interpolation >= 1) {
        ptrdiff_t len = (intptr_t)iio_buffer_end(buf) - (intptr_t)iio_buffer_start(buf);
        memset(iio_buffer_start(buf), 0, len);
    }

    size_t total = 0;
    size_t items = 1;
    while (items) {
        items = std::min((size_t)(buffer_size / (1 + interpolation)) - total,
                         std::min(INTERNAL_BUFFER_SIZE, noutput_items - total));

        for (unsigned int i = 0; i < input_items.size(); i++) {
            volk_32fc_deinterleave_32f_x2(
                f_buffer_i.data(),
                f_buffer_q.data(),
                reinterpret_cast<const gr_complex*>(input_items[i]) + total,
                items);

            volk_32f_s32f_convert_16i(
                s_buffer_i.data(), f_buffer_i.data(), 32768.f, items);
            volk_32f_s32f_convert_16i(
                s_buffer_q.data(), f_buffer_q.data(), 32768.f, items);

            channel_write(
                channel_list[2 * i], s_buffer_i.data(), total, items * sizeof(int16_t));
            channel_write(channel_list[2 * i + 1],
                          s_buffer_q.data(),
                          total,
                          items * sizeof(int16_t));
        }

        total += items;
    }

    ret = iio_buffer_push(buf);
    if (ret < 0) {
        char buf[256];
        iio_strerror(-ret, buf, sizeof(buf));
        std::string error(buf);

        std::cerr << "Unable to push buffer: " << error << std::endl;
        return -1; /* EOF */
    }

    consume_each(buffer_size / (interpolation + 1));
    return 0;
}


void ad9081_sink_impl::set_main_nco_freq(int nco, double freq)
{
    ad9081_common::set_main_nco_freq(nco, freq);
}

void ad9081_sink_impl::set_main_nco_phase(int nco, float phase)
{
    ad9081_common::set_main_nco_phase(nco, phase);
}

void ad9081_sink_impl::set_channel_nco_freq(int nco, double freq)
{
    ad9081_common::set_channel_nco_freq(nco, freq);
}

void ad9081_sink_impl::set_channel_nco_phase(int nco, float phase)
{
    ad9081_common::set_channel_nco_phase(nco, phase);
}

} /* namespace iio */
} /* namespace gr */
