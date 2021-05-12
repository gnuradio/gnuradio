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
#include <gnuradio/block.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/buffer_single_mapped.h>
#include <gnuradio/integer_math.h>
#include <gnuradio/math.h>
#include <gnuradio/thread/thread.h>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace gr {

buffer_single_mapped::buffer_single_mapped(int nitems,
                                           size_t sizeof_item,
                                           uint64_t downstream_lcm_nitems,
                                           block_sptr link,
                                           block_sptr buf_owner)
    : buffer(BufferMappingType::SingleMapped,
             nitems,
             sizeof_item,
             downstream_lcm_nitems,
             link),
      d_buf_owner(buf_owner),
      d_buffer(nullptr,
               std::bind(&buffer_single_mapped::deleter, this, std::placeholders::_1))
{
    gr::configure_default_loggers(d_logger, d_debug_logger, "buffer_single_mapped");
    if (!allocate_buffer(nitems, sizeof_item, downstream_lcm_nitems))
        throw std::bad_alloc();

#ifdef BUFFER_DEBUG
    // BUFFER DEBUG
    {
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "buffer_single_mapped constructor -- history: " << link->history();
        GR_LOG_DEBUG(d_logger, msg.str());
    }
#endif
}

buffer_single_mapped::~buffer_single_mapped() {}

/*!
 * Allocates underlying buffer.
 * returns true iff successful.
 */
bool buffer_single_mapped::allocate_buffer(int nitems,
                                           size_t sizeof_item,
                                           uint64_t downstream_lcm_nitems)
{
#ifdef BUFFER_DEBUG
    int orig_nitems = nitems;
#endif

    // Unlike the double mapped buffer case that can easily wrap back onto itself
    // for both reads and writes the single mapped case needs to be aware of read
    // and write granularity and size the underlying buffer accordingly. Otherwise
    // the calls to space_available() and items_available() may return values that
    // are too small and the scheduler will get stuck.
    uint64_t write_granularity = 1;

    if (link()->fixed_rate()) {
        // Fixed rate
        int num_inputs =
            link()->fixed_rate_noutput_to_ninput(1) - (link()->history() - 1);
        write_granularity =
            link()->fixed_rate_ninput_to_noutput(num_inputs + (link()->history() - 1));
    }

    if (link()->relative_rate() != 1.0) {
        // Some blocks say they have fixed rate but actually have a relative
        // rate set (looking at you puncture_bb...) so make this a separate
        // check.

        // Relative rate
        write_granularity = link()->relative_rate_i();
    }

    // If the output multiple has been set explicitly then adjust the write
    // granularity.
    if (link()->output_multiple_set()) {
        write_granularity =
            GR_LCM(write_granularity, (uint64_t)link()->output_multiple());
    }

#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "WRITE GRANULARITY: " << write_granularity;
    GR_LOG_DEBUG(d_logger, msg.str());
#endif

    // Adjust size so output buffer size is a multiple of the write granularity
    if (write_granularity != 1 || downstream_lcm_nitems != 1) {
        uint64_t size_align_adjust = GR_LCM(write_granularity, downstream_lcm_nitems);
        uint64_t remainder = nitems % size_align_adjust;
        nitems += (remainder > 0) ? (size_align_adjust - remainder) : 0;

#ifdef BUFFER_DEBUG
        std::ostringstream msg;
        msg << "allocate_buffer()** called  nitems: " << orig_nitems
            << " -- read_multiple: " << downstream_lcm_nitems
            << " -- write_multiple: " << write_granularity
            << " -- NEW nitems: " << nitems;
        GR_LOG_DEBUG(d_logger, msg.str());
#endif
    }

    // Allocate a new custom buffer from the owning block
    char* buf = buf_owner()->allocate_custom_buffer(nitems * sizeof_item);
    assert(buf != nullptr);
    d_buffer.reset(buf);

    d_base = d_buffer.get();
    d_bufsize = nitems;

    d_downstream_lcm_nitems = downstream_lcm_nitems;
    d_write_multiple = write_granularity;

    return true;
}

bool buffer_single_mapped::output_blkd_cb_ready(int output_multiple)
{
    uint32_t space_avail = 0;
    {
        gr::thread::scoped_lock(*this->mutex());
        space_avail = space_available();
    }
    return ((space_avail > 0) &&
            ((space_avail / output_multiple) * output_multiple == 0));
}


bool buffer_single_mapped::output_blocked_callback(int output_multiple, bool force)
{
    uint32_t space_avail = space_available();

#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "[" << this << "] "
        << "output_blocked_callback()*** WR_idx: " << d_write_index
        << " -- WR items: " << nitems_written()
        << " -- output_multiple: " << output_multiple
        << " -- space_avail: " << space_avail << " -- force: " << force;
    GR_LOG_DEBUG(d_logger, msg.str());
#endif

    if (((space_avail > 0) && ((space_avail / output_multiple) * output_multiple == 0)) ||
        force) {
        // Find reader with the smallest read index
        uint32_t min_read_idx = d_readers[0]->d_read_index;
        for (size_t idx = 1; idx < d_readers.size(); ++idx) {
            // Record index of reader with minimum read-index
            if (d_readers[idx]->d_read_index < min_read_idx) {
                min_read_idx = d_readers[idx]->d_read_index;
            }
        }

#ifdef BUFFER_DEBUG
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "output_blocked_callback() WR_idx: " << d_write_index
            << " -- WR items: " << nitems_written() << " -- min RD_idx: " << min_read_idx
            << " -- shortcircuit: "
            << ((min_read_idx == 0) || (min_read_idx >= d_write_index))
            << " -- to_move_items: " << (d_write_index - min_read_idx)
            << " -- space_avail: " << space_avail << " -- force: " << force;
        GR_LOG_DEBUG(d_logger, msg.str());
#endif

        // Make sure we have enough room to start writing back at the beginning
        if ((min_read_idx == 0) || (min_read_idx >= d_write_index)) {
            return false;
        }

        // Determine how much "to be read" data needs to be moved
        int to_move_items = d_write_index - min_read_idx;
        assert(to_move_items > 0);
        uint32_t to_move_bytes = to_move_items * d_sizeof_item;

        // Shift "to be read" data back to the beginning of the buffer
        std::memmove(d_base, d_base + (min_read_idx * d_sizeof_item), to_move_bytes);

        // Adjust write index and each reader index
        d_write_index -= min_read_idx;

        for (size_t idx = 0; idx < d_readers.size(); ++idx) {
            d_readers[idx]->d_read_index -= min_read_idx;
        }

        return true;
    }

    return false;
}

int buffer_single_mapped::space_available()
{
    if (d_readers.empty())
        return d_bufsize;

    else {

        size_t min_items_read_idx = 0;
        uint64_t min_items_read = d_readers[0]->nitems_read();
        for (size_t idx = 1; idx < d_readers.size(); ++idx) {
            // Record index of reader with minimum nitems read
            if (d_readers[idx]->nitems_read() <
                d_readers[min_items_read_idx]->nitems_read()) {
                min_items_read_idx = idx;
            }
            min_items_read = std::min(min_items_read, d_readers[idx]->nitems_read());
        }

        buffer_reader* min_idx_reader = d_readers[min_items_read_idx];
        unsigned min_read_index = d_readers[min_items_read_idx]->d_read_index;

        // For single mapped buffer there is no wrapping beyond the end of the
        // buffer
#ifdef BUFFER_DEBUG
        int thecase = 4; // REMOVE ME - just for debug
#endif
        int space = d_bufsize - d_write_index;

        if (min_read_index == d_write_index) {
#ifdef BUFFER_DEBUG
            thecase = 1;
#endif

            // If the (min) read index and write index are equal then the buffer
            // is either completely empty or completely full depending on if
            // the number of items read matches the number written
            size_t offset = ((min_idx_reader->link()->history() - 1) +
                             min_idx_reader->sample_delay());
            if ((min_idx_reader->nitems_read() - offset) != nitems_written()) {
#ifdef BUFFER_DEBUG
                thecase = 2;
#endif
                space = 0;
            }
        } else if (min_read_index > d_write_index) {
#ifdef BUFFER_DEBUG
            thecase = 3;
#endif
            space = min_read_index - d_write_index;
            // Leave extra space in case the reader gets stuck and needs realignment
            {
                if ((d_write_index > (d_bufsize / 2)) ||
                    (min_read_index < (d_bufsize / 2))) {
#ifdef BUFFER_DEBUG
                    thecase = 17;
#endif
                    space = 0;
                } else {
                    space = (d_bufsize / 2) - d_write_index;
                }
            }
        }

        if (min_items_read != d_last_min_items_read) {
            prune_tags(d_last_min_items_read);
            d_last_min_items_read = min_items_read;
        }

#ifdef BUFFER_DEBUG
        // BUFFER DEBUG
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "space_available() called  (case: " << thecase
            << ")  d_write_index: " << d_write_index << " (" << nitems_written() << ") "
            << " -- min_read_index: " << min_read_index << " ("
            << min_idx_reader->nitems_read() << ") "
            << " -- space: " << space
            << " (sample delay: " << min_idx_reader->sample_delay() << ")";
        GR_LOG_DEBUG(d_logger, msg.str());
#endif

        return space;
    }
}

} /* namespace gr */
