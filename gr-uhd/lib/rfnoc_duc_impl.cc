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

#include "rfnoc_duc_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace uhd {

rfnoc_duc::sptr rfnoc_duc::make(rfnoc_graph::sptr graph,
                                const ::uhd::device_addr_t& block_args,
                                const int device_select,
                                const int instance)
{
    return gnuradio::make_block_sptr<rfnoc_duc_impl>(
        rfnoc_block::make_block_ref(graph, block_args, "DUC", device_select, instance));
}


rfnoc_duc_impl::rfnoc_duc_impl(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : rfnoc_block(block_ref), d_duc_ref(get_block_ref<::uhd::rfnoc::duc_block_control>())
{
}

rfnoc_duc_impl::~rfnoc_duc_impl() {}

/******************************************************************************
 * rfnoc_duc API
 *****************************************************************************/
double rfnoc_duc_impl::set_freq(const double freq,
                                const size_t chan,
                                const ::uhd::time_spec_t time)
{
    return d_duc_ref->set_freq(freq, chan, time);
}

double rfnoc_duc_impl::set_input_rate(const double rate, const size_t chan)
{
    return d_duc_ref->set_input_rate(rate, chan);
}

} /* namespace uhd */
} /* namespace gr */
