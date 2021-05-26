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

#include "ad9081_source_impl.h"

#include <gnuradio/io_signature.h>
#include <volk/volk.h>

#include <array>
#include <fstream>
#include <string>
#include <vector>

namespace gr {
namespace iio {

ad9081_source::sptr ad9081_source::make(const std::string& uri,
                                        const std::array<bool, MAX_CHANNEL_COUNT>& en,
                                        size_t buffer_size)
{
    return gnuradio::make_block_sptr<ad9081_source_impl>(
        // TODO: move get_context() from device_source_impl to a common location
        device_source_impl::get_context(uri),
        en,
        buffer_size);
}

ad9081_source_impl::ad9081_source_impl(struct iio_context* ctx,
                                       const std::array<bool, MAX_CHANNEL_COUNT>& en,
                                       size_t buffer_size)
    : gr::sync_block("ad9081_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, -1, sizeof(gr_complex))),
      device_source_impl(ctx,
                         destroy_ctx,
                         "axi-ad9081-rx-hpc",
                         get_channel_vector(en),
                         "axi-ad9081-rx-hpc",
                         std::vector<std::string>(),
                         buffer_size,
                         0)
{
    for (int i = 0; i < MAX_CHANNEL_COUNT; i++) {
        auto& st = d_channel_state[i];
        st.enabled = en[i];

        if (!st.enabled)
            continue;

        d_last_active_channel = i;

        std::string ch_i_name = "voltage" + std::to_string(i) + "_i";
        std::string ch_q_name = "voltage" + std::to_string(i) + "_q";

        st.ch_i = iio_device_find_channel(dev, ch_i_name.c_str(), false);
        st.ch_q = iio_device_find_channel(dev, ch_q_name.c_str(), false);

        if (!st.ch_i || !st.ch_q)
            throw std::runtime_error("ad9081: Failed to acquire channel voltage" +
                                     std::to_string(i) + "_[iq]!");

        long long int converter_rate;
        auto ret =
            iio_channel_attr_read_longlong(st.ch_i, "adc_frequency", &converter_rate);
        if (ret)
            throw std::runtime_error(
                "ad9081: Failed to read adc_frequency from iio channel!");

        st.converter_rate = static_cast<int64_t>(converter_rate);

        parse_datapath(st);

        d_channel_ncos[st.channel_nco] = (int8_t)i;
        d_main_ncos[st.main_nco] = (int8_t)i;
    }

    if (d_last_active_channel == -1)
        throw std::runtime_error("ad9081: No channels active!");
}

void ad9081_source_impl::channel_read(const struct iio_channel* chn,
                                      void* dst,
                                      size_t len)
{
    uintptr_t src_ptr, dst_ptr = (uintptr_t)dst, end = dst_ptr + len;
    unsigned int length = iio_channel_get_data_format(chn)->length / 8;
    uintptr_t buf_end = (uintptr_t)iio_buffer_end(buf);
    ptrdiff_t buf_step = iio_buffer_step(buf) * (decimation + 1);

    for (src_ptr = (uintptr_t)iio_buffer_first(buf, chn) + byte_offset;
         src_ptr < buf_end && dst_ptr + length <= end;
         src_ptr += buf_step, dst_ptr += length)
        iio_channel_convert(chn, (void*)dst_ptr, (const void*)src_ptr);
}

int ad9081_source_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    ssize_t ret;

    // Check if we've processed what we have first
    if (!items_in_buffer) {
        ret = iio_buffer_refill(buf);
        if (ret < 0) {
            /* -EBADF happens when the buffer is cancelled */
            if (ret != -EBADF) {

                char buf[256];
                iio_strerror(-ret, buf, sizeof(buf));
                std::string error(buf);

                std::cerr << "Unable to refill buffer: " << error << std::endl;
            }
            return -1;
        }

        items_in_buffer = (unsigned long)ret / iio_buffer_step(buf);
        if (!items_in_buffer)
            return 0;

        byte_offset = 0;
    }

    size_t total = 0;
    size_t items = 1;

    while (items) {
        items =
            std::min(INTERNAL_BUFFER_SIZE,
                     std::min(items_in_buffer, (unsigned long)(noutput_items - total)));

        for (unsigned int i = 0; i < output_items.size(); i++) {
            channel_read(channel_list[2 * i], s_buffer_i.data(), items * sizeof(short));
            channel_read(
                channel_list[2 * i + 1], s_buffer_q.data(), items * sizeof(short));

            volk_16i_s32f_convert_32f(
                f_buffer_i.data(), s_buffer_i.data(), 8192.f, items);
            volk_16i_s32f_convert_32f(
                f_buffer_q.data(), s_buffer_q.data(), 8192.f, items);
            volk_32f_x2_interleave_32fc(reinterpret_cast<gr_complex*>(output_items[i]) +
                                            total,
                                        f_buffer_i.data(),
                                        f_buffer_q.data(),
                                        items);
        }

        items_in_buffer -= items;
        byte_offset += items * iio_buffer_step(buf) * (1 + decimation);
        total += items;
    }

    return (int)total;
}

void ad9081_source_impl::set_nyquist_zone(bool odd)
{
    auto& st = d_channel_state[d_last_active_channel];
    int ret = iio_channel_attr_write(st.ch_i, "nyquist_zone", odd ? "odd" : "even");
    if (ret < 0)
        throw std::runtime_error(
            "ad9081: set_nyquist_zone: Failed to write NCO attribute");
}

void ad9081_source_impl::set_filter_source_file(const std::string& filter)
{
    if (filter.empty() || !iio_device_find_attr(phy, "filter_fir_config"))
        return;

    std::ifstream ifs(filter.c_str(), std::ifstream::binary);
    if (!ifs)
        throw std::runtime_error("ad9081: Failed to open filter file!");

    ifs.seekg(0, ifs.end);
    size_t length = ifs.tellg();
    ifs.seekg(0, ifs.beg);

    char* buffer = new char[length];

    ifs.read(buffer, length);
    ifs.close();

    int ret = iio_device_attr_write_raw(phy, "filter_fir_config", buffer, length);

    delete[] buffer;
    if (ret < 0)
        throw std::runtime_error("ad9081: Failed to write filter config to device");
}

void ad9081_source_impl::set_main_nco_freq(int nco, double freq)
{
    ad9081_common::set_main_nco_freq(nco, freq);
}

void ad9081_source_impl::set_main_nco_phase(int nco, float phase)
{
    ad9081_common::set_main_nco_phase(nco, phase);
}

void ad9081_source_impl::set_channel_nco_freq(int nco, double freq)
{
    ad9081_common::set_channel_nco_freq(nco, freq);
}

void ad9081_source_impl::set_channel_nco_phase(int nco, float phase)
{
    ad9081_common::set_channel_nco_phase(nco, phase);
}

} /* namespace iio */
} /* namespace gr */
