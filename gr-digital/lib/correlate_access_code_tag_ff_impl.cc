/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010-2012,2017 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "correlate_access_code_tag_ff_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <volk/volk.h>
#include <boost/format.hpp>
#include <cstdio>
#include <stdexcept>

namespace gr {
namespace digital {

correlate_access_code_tag_ff::sptr correlate_access_code_tag_ff::make(
    const std::string& access_code, int threshold, const std::string& tag_name)
{
    return gnuradio::make_block_sptr<correlate_access_code_tag_ff_impl>(
        access_code, threshold, tag_name);
}


correlate_access_code_tag_ff_impl::correlate_access_code_tag_ff_impl(
    const std::string& access_code, int threshold, const std::string& tag_name)
    : sync_block("correlate_access_code_tag_ff",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make(1, 1, sizeof(float))),
      d_data_reg(0),
      d_mask(0),
      d_threshold(threshold),
      d_len(0)
{
    if (!set_access_code(access_code)) {
        GR_LOG_ERROR(d_logger, "access_code is > 64 bits");
        throw std::out_of_range("access_code is > 64 bits");
    }

    std::stringstream str;
    str << name() << unique_id();
    d_me = pmt::string_to_symbol(str.str());
    d_key = pmt::string_to_symbol(tag_name);
}

correlate_access_code_tag_ff_impl::~correlate_access_code_tag_ff_impl() {}

bool correlate_access_code_tag_ff_impl::set_access_code(const std::string& access_code)
{
    gr::thread::scoped_lock l(d_mutex_access_code);

    d_len = access_code.length(); // # of bytes in string
    if (d_len > 64)
        return false;

    // set len bottom bits to 1.
    d_mask = ((~0ULL) >> (64 - d_len));

    d_access_code = 0;
    for (unsigned i = 0; i < d_len; i++) {
        d_access_code = (d_access_code << 1) | (access_code[i] & 1);
    }

    GR_LOG_DEBUG(d_logger, boost::format("Access code: %llx") % d_access_code);
    GR_LOG_DEBUG(d_logger, boost::format("Mask: %llx") % d_mask);

    return true;
}

int correlate_access_code_tag_ff_impl::work(int noutput_items,
                                            gr_vector_const_void_star& input_items,
                                            gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(d_mutex_access_code);

    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];

    uint64_t abs_out_sample_cnt = nitems_written(0);

    for (int i = 0; i < noutput_items; i++) {
        out[i] = in[i];

        // compute hamming distance between desired access code and current data
        uint64_t wrong_bits = 0;
        uint64_t nwrong = d_threshold + 1;

        if (d_data_reg_bits < d_len) {
            d_data_reg_bits++;
        } else {
            wrong_bits = (d_data_reg ^ d_access_code) & d_mask;
            volk_64u_popcnt(&nwrong, wrong_bits);
        }

        // shift in new data
        d_data_reg = (d_data_reg << 1) | (gr::branchless_binary_slicer(in[i]) & 0x1);
        if (nwrong <= d_threshold) {
            GR_LOG_DEBUG(d_logger,
                         boost::format("writing tag at sample %llu") %
                             (abs_out_sample_cnt + i));
            add_item_tag(0,                      // stream ID
                         abs_out_sample_cnt + i, // sample
                         d_key,                  // frame info
                         pmt::from_long(nwrong), // data (number wrong)
                         d_me                    // block src id
            );
        }
    }

    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
