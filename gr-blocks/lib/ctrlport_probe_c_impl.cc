/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ctrlport_probe_c_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

ctrlport_probe_c::sptr ctrlport_probe_c::make(const std::string& id,
                                              const std::string& desc)
{
    return gnuradio::make_block_sptr<ctrlport_probe_c_impl>(id, desc);
}

ctrlport_probe_c_impl::ctrlport_probe_c_impl(const std::string& id,
                                             const std::string& desc)
    : sync_block("probe_c",
                 io_signature::make(1, 1, sizeof(gr_complex)),
                 io_signature::make(0, 0, 0)),
      d_id(id),
      d_desc(desc),
      d_ptr(NULL),
      d_ptrLen(0)
{
}

ctrlport_probe_c_impl::~ctrlport_probe_c_impl() {}

std::vector<gr_complex> ctrlport_probe_c_impl::get()
{
    if (d_ptr != NULL && d_ptrLen > 0) {
        ptrlock.lock();
        std::vector<gr_complex> vec(d_ptr, d_ptr + d_ptrLen);
        ptrlock.unlock();
        return vec;
    } else {
        std::vector<gr_complex> vec;
        return vec;
    }
}

int ctrlport_probe_c_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];

    // keep reference to symbols
    ptrlock.lock();
    d_ptr = in;
    d_ptrLen = noutput_items;
    ptrlock.unlock();

    return noutput_items;
}

void ctrlport_probe_c_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    d_rpc_vars.emplace_back(
        new rpcbasic_register_get<ctrlport_probe_c, std::vector<std::complex<float>>>(
            alias(),
            d_id.c_str(),
            &ctrlport_probe_c::get,
            pmt::make_c32vector(0, -2),
            pmt::make_c32vector(0, 2),
            pmt::make_c32vector(0, 0),
            "volts",
            d_desc.c_str(),
            RPC_PRIVLVL_MIN,
            DISPXY | DISPOPTSCATTER));
#endif /* GR_CTRLPORT */
}

} /* namespace blocks */
} /* namespace gr */
