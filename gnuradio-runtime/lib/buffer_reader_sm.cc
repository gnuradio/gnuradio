/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gnuradio/block.h>
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_reader_sm.h>
#include <gnuradio/math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace gr {

buffer_reader_sm::~buffer_reader_sm() {}

int buffer_reader_sm::items_available() const
{
    int available = 0;

    if ((nitems_read() - sample_delay()) != d_buffer->nitems_written()) {
        if (d_buffer->d_write_index == d_read_index) {
            if ((nitems_read() - sample_delay()) !=
                (d_buffer->nitems_written() + link()->history() - 1)) {
                available = d_buffer->d_bufsize - d_read_index;
            }
        } else {
            available = d_buffer->index_sub(d_buffer->d_write_index, d_read_index);
        }
    }

#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "[" << d_buffer << ";" << this << "] "
        << "items_available() WR_idx: " << d_buffer->d_write_index
        << " -- WR items: " << d_buffer->nitems_written()
        << " -- RD_idx: " << d_read_index << " -- RD items: " << nitems_read() << " (-"
        << d_attr_delay << ") -- available: " << available;
    GR_LOG_DEBUG(d_logger, msg.str());
#endif

    return available;
}

bool buffer_reader_sm::input_blkd_cb_ready(int items_required) const
{
    return d_buffer->input_blkd_cb_ready(items_required, d_read_index);
}

bool buffer_reader_sm::input_blocked_callback(int items_required, int items_avail)
{
    // Update items available before going farther as it could be stale
    items_avail = items_available();

    return d_buffer->input_blocked_callback(items_required, items_avail, d_read_index);
}

buffer_reader_sm::buffer_reader_sm(buffer_sptr buffer,
                                   unsigned int read_index,
                                   block_sptr link)
    : buffer_reader(buffer, read_index, link)
{
}


} /* namespace gr */
