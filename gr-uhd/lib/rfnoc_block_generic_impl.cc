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

#include "rfnoc_block_generic_impl.h"
#include <gnuradio/uhd/rfnoc_graph.h>

namespace gr {
namespace uhd {

/******************************************************************************
 * Factory and Structors
 *****************************************************************************/
rfnoc_block_generic::sptr
rfnoc_block_generic::make(rfnoc_graph::sptr graph,
                          const ::uhd::device_addr_t& block_args,
                          const std::string& block_name,
                          const int device_select,
                          const int block_select)
{
    return gnuradio::make_block_sptr<rfnoc_block_generic_impl>(
        rfnoc_block::make_block_ref(
            graph, block_args, block_name, device_select, block_select));
}

rfnoc_block_generic_impl::rfnoc_block_generic_impl(
    ::uhd::rfnoc::noc_block_base::sptr block_ref)
    : rfnoc_block(block_ref)
{
}

rfnoc_block_generic_impl::~rfnoc_block_generic_impl() {}

} /* namespace uhd */
} /* namespace gr */
