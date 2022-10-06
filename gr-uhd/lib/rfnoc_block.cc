/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 * Copyright 2020 Ettus Research, A National Instruments Brand.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/uhd/rfnoc_block.h>

namespace gr {
namespace uhd {

/******************************************************************************
 * Factory and Structors
 *****************************************************************************/
::uhd::rfnoc::noc_block_base::sptr
rfnoc_block::make_block_ref(rfnoc_graph::sptr graph,
                            const ::uhd::device_addr_t& block_args,
                            const std::string& block_name,
                            const int device_select,
                            const int block_select,
                            const size_t max_ref_count)
{
    const std::string block_id =
        graph->get_block_id(block_name, device_select, block_select);
    if (block_id.empty()) {
        throw std::runtime_error("Cannot find block!");
    }

    auto block = graph->get_block_ref(block_id, max_ref_count);
    if (block) {
        block->set_properties(block_args, 0);
    }

    return block;
}

rfnoc_block::rfnoc_block(::uhd::rfnoc::noc_block_base::sptr block_ref)
    : gr::block(
          std::string("RFNoC::") + block_ref->get_unique_id(),
          gr::io_signature::make(0, 0, 0), // All RFNoC blocks don't stream into GNU Radio
          gr::io_signature::make(0, 0, 0)),
      d_block_ref(block_ref)
{
}

/******************************************************************************
 * GNU Radio API
 *****************************************************************************/
std::string rfnoc_block::get_unique_id() const { return d_block_ref->get_unique_id(); }

int rfnoc_block::general_work(int /*noutput_items*/,
                              gr_vector_int& /*ninput_items*/,
                              gr_vector_const_void_star& /*input_items*/,
                              gr_vector_void_star& /*output_items*/)
{
    // We should never land here
    throw std::runtime_error("Unexpected call to general_work() in an RFNoC block!");
    return 0;
}

std::vector<std::string> rfnoc_block::get_property_ids()
{
    return d_block_ref->get_property_ids();
}

} /* namespace uhd */
} /* namespace gr */
