/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "null_sink_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

null_sink::sptr null_sink::make(size_t sizeof_stream_item)
{
    return gnuradio::make_block_sptr<null_sink_impl>(sizeof_stream_item);
}

null_sink_impl::null_sink_impl(size_t sizeof_stream_item)
    : sync_block("null_sink",
                 io_signature::make(1, -1, sizeof_stream_item),
                 io_signature::make(0, 0, 0))
{
}

null_sink_impl::~null_sink_impl() {}

int null_sink_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
