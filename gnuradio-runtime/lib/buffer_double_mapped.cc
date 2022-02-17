/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "vmcircbuf.h"
#include <gnuradio/block.h>
#include <gnuradio/buffer_double_mapped.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <stdexcept>

namespace gr {

/*
 * Compute the minimum number of buffer items that work (i.e.,
 * address space wrap-around works).  To work is to satisfy this
 * constraint for integer buffer_size and k:
 *
 *     type_size * nitems == k * page_size
 */
static inline long minimum_buffer_items(size_t type_size, size_t page_size)
{
    return page_size / std::gcd(type_size, page_size);
}

buffer_type
    buffer_double_mapped::type(buftype<buffer_double_mapped, buffer_double_mapped>{});

buffer_double_mapped::buffer_double_mapped(int nitems,
                                           size_t sizeof_item,
                                           uint64_t downstream_lcm_nitems,
                                           uint32_t downstream_max_out_mult,
                                           block_sptr link)
    : buffer(buffer_mapping_type::double_mapped,
             nitems,
             sizeof_item,
             downstream_lcm_nitems,
             downstream_max_out_mult,
             link)
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "buffer_double_mapped");
    if (!allocate_buffer(nitems))
        throw std::bad_alloc();

#ifdef BUFFER_DEBUG
    {
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "buffer_double_mapped constructor -- history: " << link->history();
        d_logger->debug(msg.str());
    }
#endif
}

// NB: Added the extra 'block_sptr unused' parameter so that the
// call signature matches the other factory-like functions used to create
// the buffer_single_mapped subclasses
buffer_sptr buffer_double_mapped::make_buffer(int nitems,
                                              size_t sizeof_item,
                                              uint64_t downstream_lcm_nitems,
                                              uint32_t downstream_max_out_mult,
                                              block_sptr link,
                                              block_sptr unused)
{
    return buffer_sptr(new buffer_double_mapped(
        nitems, sizeof_item, downstream_lcm_nitems, downstream_max_out_mult, link));
}

buffer_double_mapped::~buffer_double_mapped() {}

/*!
 * sets d_vmcircbuf, d_base, d_bufsize.
 * returns true iff successful.
 */
bool buffer_double_mapped::allocate_buffer(int nitems)
{
    int orig_nitems = nitems;

    // Any buffer size we come up with must be a multiple of min_nitems.
    int granularity = gr::vmcircbuf_sysconfig::granularity();
    int min_nitems = minimum_buffer_items(d_sizeof_item, granularity);

    // Round-up nitems to a multiple of min_nitems.
    if (nitems % min_nitems != 0)
        nitems = ((nitems / min_nitems) + 1) * min_nitems;

    // If we rounded-up a whole bunch, give the user a heads up.
    // This only happens if sizeof_item is not a power of two.
    if (nitems > 2 * orig_nitems && nitems * (int)d_sizeof_item > granularity) {
        d_logger->warn("allocate_buffer: tried to allocate"
                       "   {:d} items of size {:d}. Due to alignment requirements"
                       "   {:d} were allocated.  If this isn't OK, consider padding"
                       "   your structure to a power-of-two bytes."
                       "   On this platform, our allocation granularity is {:d} bytes.",
                       orig_nitems,
                       d_sizeof_item,
                       nitems,
                       granularity);
    }

    d_bufsize = nitems;
    d_vmcircbuf.reset(gr::vmcircbuf_sysconfig::make(d_bufsize * d_sizeof_item));
    if (d_vmcircbuf == 0) {
        d_logger->error(
            "gr::buffer::allocate_buffer: failed to allocate buffer of size {:d} KB",
            d_bufsize * d_sizeof_item / 1024);
        return false;
    }

    d_base = (char*)d_vmcircbuf->pointer_to_first_copy();
    return true;
}

int buffer_double_mapped::space_available()
{
    if (d_readers.empty())
        return d_bufsize - 1; // See comment below

    else {

        // Find out the maximum amount of data available to our readers
        int most_data = d_readers[0]->items_available();
        uint64_t min_items_read = d_readers[0]->nitems_read();
        for (size_t i = 1; i < d_readers.size(); i++) {
            most_data = std::max(most_data, d_readers[i]->items_available());
            min_items_read = std::min(min_items_read, d_readers[i]->nitems_read());
        }

        if (min_items_read != d_last_min_items_read) {
            prune_tags(d_last_min_items_read);
            d_last_min_items_read = min_items_read;
        }

#ifdef BUFFER_DEBUG
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "space_available() called  d_write_index: " << d_write_index
            << " -- space_available: " << (d_bufsize - most_data - 1);
        d_logger->debug(msg.str());
#endif

        // The -1 ensures that the case d_write_index == d_read_index is
        // unambiguous.  It indicates that there is no data for the reader
        return d_bufsize - most_data - 1;
    }
}

} /* namespace gr */
