/* -*- c++ -*- */
/*
 * Copyright 2019 Ettus Research, a National Instruments Brand.
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rfnoc_ddc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace uhd {

rfnoc_ddc::sptr rfnoc_ddc::make(rfnoc_graph::sptr graph,
                                const ::uhd::device_addr_t& block_args,
                                const int device_select,
                                const int instance)
{
    return gnuradio::make_block_sptr<rfnoc_ddc_impl>(
        rfnoc_block::make_block_ref(graph, block_args, "DDC", device_select, instance));
}


rfnoc_ddc_impl::rfnoc_ddc_impl(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : rfnoc_block(block_ref), d_ddc_ref(get_block_ref<::uhd::rfnoc::ddc_block_control>())
{
}

rfnoc_ddc_impl::~rfnoc_ddc_impl() {}

/******************************************************************************
 * rfnoc_ddc API
 *****************************************************************************/
double rfnoc_ddc_impl::set_freq(const double freq,
                                const size_t chan,
                                const ::uhd::time_spec_t time)
{
    return d_ddc_ref->set_freq(freq, chan, time);
}

double rfnoc_ddc_impl::set_output_rate(const double rate, const size_t chan)
{
    return d_ddc_ref->set_output_rate(rate, chan);
}

} /* namespace uhd */
} /* namespace gr */
