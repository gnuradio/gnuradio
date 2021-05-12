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
#include <gnuradio/integer_math.h>
#include <gnuradio/math.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace gr {

buffer_reader_sm::~buffer_reader_sm() {}

int buffer_reader_sm::items_available()
{
    int available = 0;

    if ((nitems_read() - sample_delay()) != d_buffer->nitems_written()) {
        if (d_buffer->d_write_index == d_read_index) {
            if ((nitems_read() - sample_delay()) != d_buffer->nitems_written()) {
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
    gr::thread::scoped_lock(*d_buffer->mutex());

    return (((d_buffer->d_bufsize - d_read_index) < (uint32_t)items_required) &&
            (d_buffer->d_write_index < d_read_index));
}

bool buffer_reader_sm::input_blocked_callback(int items_required, int items_avail)
{
    // Maybe adjust read pointers from min read index?
    // This would mean that *all* readers must be > (passed) the write index
    if (((d_buffer->d_bufsize - d_read_index) < (uint32_t)items_required) &&
        (d_buffer->d_write_index < d_read_index)) {

        // Update items available before going farther as it could be stale
        items_avail = items_available();

        // Find reader with the smallest read index that is greater than the
        // write index
        uint32_t min_reader_index = std::numeric_limits<uint32_t>::max();
        uint32_t min_read_idx = std::numeric_limits<uint32_t>::max();
        for (size_t idx = 0; idx < d_buffer->d_readers.size(); ++idx) {
            if (d_buffer->d_readers[idx]->d_read_index > d_buffer->d_write_index) {
                // Record index of reader with minimum read-index
                if (d_buffer->d_readers[idx]->d_read_index < min_read_idx) {
                    min_read_idx = d_buffer->d_readers[idx]->d_read_index;
                    min_reader_index = idx;
                }
            }
        }

        // Note items_avail might be zero, that's okay.
        items_avail += d_read_index - min_read_idx;
        int gap = min_read_idx - d_buffer->d_write_index;
        if (items_avail > gap) {
            return false;
        }

#ifdef BUFFER_DEBUG
        std::ostringstream msg;
        msg << "[" << d_buffer << ";" << this << "] "
            << "input_blocked_callback() WR_idx: " << d_buffer->d_write_index
            << " -- WR items: " << d_buffer->nitems_written()
            << " -- BUFSIZE: " << d_buffer->d_bufsize << " -- RD_idx: " << min_read_idx;
        for (size_t idx = 0; idx < d_buffer->d_readers.size(); ++idx) {
            if (idx != min_reader_index) {
                msg << " -- OTHER_RDR: " << d_buffer->d_readers[idx]->d_read_index;
            }
        }

        msg << " -- GAP: " << gap << " -- items_required: " << items_required
            << " -- items_avail: " << items_avail;
        GR_LOG_DEBUG(d_logger, msg.str());
#endif

        // Shift existing data down to make room for blocked data at end of buffer
        uint32_t move_data_size = d_buffer->d_write_index * d_buffer->d_sizeof_item;
        char* dest = d_buffer->d_base + (items_avail * d_buffer->d_sizeof_item);
        std::memmove(dest, d_buffer->d_base, move_data_size);

        // Next copy the data from the end of the buffer back to the beginning
        uint32_t avail_data_size = items_avail * d_buffer->d_sizeof_item;
        char* src = d_buffer->d_base + (min_read_idx * d_buffer->d_sizeof_item);
        std::memcpy(d_buffer->d_base, src, avail_data_size);

        // Now adjust write pointer
        d_buffer->d_write_index += items_avail;

        // Finally adjust all reader pointers
        for (size_t idx = 0; idx < d_buffer->d_readers.size(); ++idx) {
            if (idx == min_reader_index) {
                d_buffer->d_readers[idx]->d_read_index = 0;
            } else {
                d_buffer->d_readers[idx]->d_read_index += items_avail;
                d_buffer->d_readers[idx]->d_read_index %= d_buffer->d_bufsize;
            }
        }

        return true;
    }

    return false;
}

buffer_reader_sm::buffer_reader_sm(buffer_sptr buffer,
                                   unsigned int read_index,
                                   block_sptr link)
    : buffer_reader(buffer, read_index, link)
{
}


} /* namespace gr */
