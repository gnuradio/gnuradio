/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ctrlport_probe2_i_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

ctrlport_probe2_i::sptr ctrlport_probe2_i::make(const std::string& id,
                                                const std::string& desc,
                                                int len,
                                                unsigned int disp_mask)
{
    return gnuradio::make_block_sptr<ctrlport_probe2_i_impl>(id, desc, len, disp_mask);
}

ctrlport_probe2_i_impl::ctrlport_probe2_i_impl(const std::string& id,
                                               const std::string& desc,
                                               int len,
                                               unsigned int disp_mask)
    : sync_block(
          "probe2_i", io_signature::make(1, 1, sizeof(int)), io_signature::make(0, 0, 0)),
      d_id(id),
      d_desc(desc),
      d_len(len),
      d_disp_mask(disp_mask)
{
    set_length(len);
}

ctrlport_probe2_i_impl::~ctrlport_probe2_i_impl() {}

void ctrlport_probe2_i_impl::forecast(int noutput_items,
                                      gr_vector_int& ninput_items_required)
{
    // make sure all inputs have noutput_items available
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = d_len;
}

std::vector<int> ctrlport_probe2_i_impl::get() { return buffered_get.get(); }

void ctrlport_probe2_i_impl::set_length(int len)
{
    gr::thread::scoped_lock guard(d_setlock);

    if (len > 8191) {
        d_logger->warn("length {:d} exceeds maximum buffer size of 8191", len);
        len = 8191;
    }

    d_len = len;
    d_buffer.resize(d_len);
    d_index = 0;
}

int ctrlport_probe2_i_impl::length() const { return (int)d_len; }

int ctrlport_probe2_i_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const int* in = (const int*)input_items[0];

    gr::thread::scoped_lock guard(d_setlock);

    // copy samples to get buffer if we need samples
    if (d_index < d_len) {
        // copy smaller of remaining buffer space and num inputs to work()
        int num_copy = std::min((int)(d_len - d_index), noutput_items);

        memcpy(&d_buffer[d_index], in, num_copy * sizeof(int));
        d_index += num_copy;
    }

    // notify the waiting get() if we fill up the buffer
    if (d_index == d_len) {
        buffered_get.offer_data(d_buffer);
        d_index = 0;
    }

    return noutput_items;
}

void ctrlport_probe2_i_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    int len = static_cast<int>(d_len);
    d_rpc_vars.emplace_back(
        new rpcbasic_register_get<ctrlport_probe2_i, std::vector<int>>(
            alias(),
            d_id.c_str(),
            &ctrlport_probe2_i::get,
            pmt::mp(-32768),
            pmt::mp(32767),
            pmt::mp(0),
            "volts",
            d_desc.c_str(),
            RPC_PRIVLVL_MIN,
            d_disp_mask));

    d_rpc_vars.emplace_back(
        new rpcbasic_register_get<ctrlport_probe2_i, int>(alias(),
                                                          "length",
                                                          &ctrlport_probe2_i::length,
                                                          pmt::mp(1),
                                                          pmt::mp(10 * len),
                                                          pmt::mp(len),
                                                          "samples",
                                                          "get vector length",
                                                          RPC_PRIVLVL_MIN,
                                                          DISPNULL));

    d_rpc_vars.emplace_back(
        new rpcbasic_register_set<ctrlport_probe2_i, int>(alias(),
                                                          "length",
                                                          &ctrlport_probe2_i::set_length,
                                                          pmt::mp(1),
                                                          pmt::mp(10 * len),
                                                          pmt::mp(len),
                                                          "samples",
                                                          "set vector length",
                                                          RPC_PRIVLVL_MIN,
                                                          DISPNULL));
#endif /* GR_CTRLPORT */
}

} /* namespace blocks */
} /* namespace gr */
