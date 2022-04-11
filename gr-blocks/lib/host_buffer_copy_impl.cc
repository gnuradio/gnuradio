/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "host_buffer_copy_impl.h"
#include <gnuradio/host_buffer.h>
#include <gnuradio/io_signature.h>

#include <cstring>

namespace gr {
namespace blocks {

using input_type = float;
using output_type = float;
host_buffer_copy::sptr host_buffer_copy::make(int history)
{
    return gnuradio::make_block_sptr<host_buffer_copy_impl>(history);
}

host_buffer_copy_impl::host_buffer_copy_impl(int history)
    : sync_block("host_buffer_copy",
                 gr::io_signature::make(1, 1, sizeof(input_type), host_buffer::type),
                 gr::io_signature::make(1, 1, sizeof(output_type), host_buffer::type))
{
    set_history(history);
}

host_buffer_copy_impl::~host_buffer_copy_impl() {}

int host_buffer_copy_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    std::memcpy(out, in, sizeof(input_type) * noutput_items);

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
