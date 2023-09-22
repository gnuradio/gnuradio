/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "stream_to_streams_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>

namespace gr {
namespace blocks {

stream_to_streams::sptr stream_to_streams::make(size_t itemsize, size_t nstreams)
{
    return gnuradio::make_block_sptr<stream_to_streams_impl>(itemsize, nstreams);
}

stream_to_streams_impl::stream_to_streams_impl(size_t itemsize, size_t nstreams)
    : sync_decimator("stream_to_streams",
                     io_signature::make(1, 1, itemsize),
                     io_signature::make(nstreams, nstreams, itemsize),
                     nstreams)
{
}

int stream_to_streams_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    size_t item_size = output_signature()->sizeof_stream_item(0);

    const char* in = (const char*)input_items[0];
    char** outv = (char**)&output_items[0];
    int nstreams = output_items.size();

    for (int i = 0; i < noutput_items; i++) {
        for (int j = 0; j < nstreams; j++) {
            memcpy(outv[j], in, item_size);
            outv[j] += item_size;
            in += item_size;
        }
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
