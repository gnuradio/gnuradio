/* -*- c++ -*- */
/*
 * Copyright 2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "pull_source_impl.h"
#include "tag_headers.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace zeromq {

pull_source::sptr pull_source::make(
    size_t itemsize, size_t vlen, char* address, int timeout, bool pass_tags, int hwm)
{
    return gnuradio::get_initial_sptr(
        new pull_source_impl(itemsize, vlen, address, timeout, pass_tags, hwm));
}

pull_source_impl::pull_source_impl(
    size_t itemsize, size_t vlen, char* address, int timeout, bool pass_tags, int hwm)
    : gr::sync_block("pull_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, 1, itemsize * vlen)),
      base_source_impl(ZMQ_PULL, itemsize, vlen, address, timeout, pass_tags, hwm)
{
    /* All is delegated */
}

int pull_source_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    uint8_t* out = (uint8_t*)output_items[0];
    bool first = true;
    int done = 0;

    /* Process as much as we can */
    while (1) {
        if (has_pending()) {
            /* Flush anything pending */
            done += flush_pending(
                out + (done * d_vsize), noutput_items - done, nitems_written(0) + done);

            /* No more space ? */
            if (done == noutput_items)
                break;
        } else {
            /* Try to get the next message */
            if (!load_message(first))
                break; /* No message, we're done for now */

            /* Not the first anymore */
            first = false;
        }
    }

    return done;
}

} /* namespace zeromq */
} /* namespace gr */

// vim: ts=2 sw=2 expandtab
