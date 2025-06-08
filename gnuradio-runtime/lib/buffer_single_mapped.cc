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
#include <gnuradio/math.h>
#include <gnuradio/thread/thread.h>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <numeric>

namespace gr {

buffer_single_mapped::buffer_single_mapped(int nitems,
                                           size_t sizeof_item,
                                           uint64_t downstream_lcm_nitems,
                                           uint32_t downstream_max_out_mult,
                                           block_sptr link,
                                           block_sptr buf_owner)
    : buffer(buffer_mapping_type::single_mapped,
             nitems,
             sizeof_item,
             downstream_lcm_nitems,
             downstream_max_out_mult,
             link),
      d_buf_owner(buf_owner),
      d_buffer(nullptr)
{
}

buffer_single_mapped::~buffer_single_mapped() {}

/*!
 * Allocates underlying buffer.
 * returns true iff successful.
 */
bool buffer_single_mapped::allocate_buffer(int nitems)
{
#ifdef BUFFER_DEBUG
    int orig_nitems = nitems;
#endif

    // For single mapped buffers resize the initial size to be at least four
    // times the size of the largest of any downstream block's output multiple.
    // This helps reduce the number of times the input_block_callback() might be
    // called which should help overall performance (particularly if the max
    // output multiple is large) at the cost of slightly more buffer space.
    if (static_cast<uint32_t>(nitems) < (4 * d_max_reader_output_multiple)) {
        nitems = 4 * d_max_reader_output_multiple;
    }

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
            std::lcm(write_granularity, (uint64_t)link()->output_multiple());
    }

#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "WRITE GRANULARITY: " << write_granularity;
    GR_LOG_DEBUG(d_logger, msg.str());
#endif

    // Adjust size so output buffer size is a multiple of the write granularity
    if (write_granularity != 1 || d_downstream_lcm_nitems != 1) {
        uint64_t size_align_adjust = std::lcm(write_granularity, d_downstream_lcm_nitems);
        uint64_t remainder = nitems % size_align_adjust;
        nitems += (remainder > 0) ? (size_align_adjust - remainder) : 0;

#ifdef BUFFER_DEBUG
        std::ostringstream msg;
        msg << "allocate_buffer()** called  nitems: " << orig_nitems
            << " -- read_multiple: " << d_downstream_lcm_nitems
            << " -- write_multiple: " << write_granularity
            << " -- NEW nitems: " << nitems;
        GR_LOG_DEBUG(d_logger, msg.str());
#endif
    }

    d_bufsize = nitems;

    d_downstream_lcm_nitems = d_downstream_lcm_nitems;
    d_write_multiple = write_granularity;

    // Do the actual allocation(s) with the finalized nitems
    return do_allocate_buffer(nitems, d_sizeof_item);
}

bool buffer_single_mapped::input_blkd_cb_ready(int items_required,
                                               unsigned int read_index)
{
    gr::thread::scoped_lock lock(*this->mutex());

    return (((d_bufsize - read_index) < (uint32_t)items_required) &&
            (d_write_index < read_index));
}

bool buffer_single_mapped::output_blkd_cb_ready(int output_multiple)
{
    uint32_t space_avail = 0;
    {
        gr::thread::scoped_lock lock(*this->mutex());
        space_avail = space_available();
    }
    return (((space_avail > 0) || (space_avail == 0 && d_has_history)) &&
            ((space_avail / output_multiple) * output_multiple == 0));
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
        int thecase = 0;
#endif
        int space = d_bufsize - d_write_index;

        // Ensure space is left to handle block history
        if (static_cast<unsigned>(space) < min_idx_reader->link()->history()) {
            space = 0;
        } else {
            space -= (min_idx_reader->link()->history() - 1);
        }

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

            // Ensure space is left to handle block history
            if (static_cast<unsigned>(space) < min_idx_reader->link()->history()) {
                space = 0;
            } else {
                space -= (min_idx_reader->link()->history() - 1);
            }

            // Leave extra space in case the reader gets stuck and needs realignment
            if (d_max_reader_output_multiple > 1) {
                if (static_cast<uint32_t>(space) > d_max_reader_output_multiple) {
#ifdef BUFFER_DEBUG
                    thecase = 4;
#endif
                    space = space - d_max_reader_output_multiple;
                } else {
#ifdef BUFFER_DEBUG
                    thecase = 5;
#endif
                    space = 0;
                }
            }
        }

        if (min_items_read != d_last_min_items_read) {
            prune_tags(d_last_min_items_read);
            d_last_min_items_read = min_items_read;
        }

#ifdef BUFFER_DEBUG
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "space_available() called  (case: " << thecase
            << ")  d_write_index: " << d_write_index << " (" << nitems_written() << ") "
            << " -- min_read_index: " << min_read_index << " ("
            << min_idx_reader->nitems_read() << ") "
            << " -- space: " << space
            << " -- max_reader_out_mult: " << d_max_reader_output_multiple
            << " (sample delay: " << min_idx_reader->sample_delay() << ")";
        GR_LOG_DEBUG(d_logger, msg.str());
#endif

        return space;
    }
}

void buffer_single_mapped::update_reader_block_history(unsigned history, int delay)
{
    unsigned old_max = d_max_reader_history;
    d_max_reader_history = std::max(d_max_reader_history, history);
    if (d_max_reader_history != old_max) {
        d_write_index = d_max_reader_history - 1;

#ifdef BUFFER_DEBUG
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "buffer_single_mapped constructor -- set wr index to: " << d_write_index;
        GR_LOG_DEBUG(d_logger, msg.str());
#endif

        // Reset the reader's read index if the buffer's write index has changed.
        // Note that "history - 1" is the nzero_preload value passed to
        // buffer_add_reader.
        for (auto reader : d_readers) {
            reader->d_read_index = d_write_index - (reader->link()->history() - 1);
        }
    }

    // Only attempt to set has history flag if it is not already set
    if (!d_has_history) {
        // Blocks that set delay may set history to delay + 1 but this is
        // not "real" history
        d_has_history = ((static_cast<int>(history) - 1) != delay);
    }
}

//------------------------------------------------------------------------------

bool buffer_single_mapped::input_blocked_callback_logic(int items_required,
                                                        int items_avail,
                                                        unsigned read_index,
                                                        char* buffer_ptr,
                                                        mem_func_t const& memcpy_func,
                                                        mem_func_t const& memmove_func)
{
#ifdef BUFFER_DEBUG
    std::ostringstream msg;
    msg << "[" << this << "] "
        << "input_blocked_callback() WR_idx: " << d_write_index
        << " -- WR items: " << nitems_written() << " -- BUFSIZE: " << d_bufsize
        << " -- RD_idx: " << read_index << " -- items_required: " << items_required
        << " -- items_avail: " << items_avail;
    GR_LOG_DEBUG(d_logger, msg.str());
#endif

    // Maybe adjust read pointers from min read index?
    // This would mean that *all* readers must be > (passed) the write index
    if (((d_bufsize - read_index) < (uint32_t)items_required) &&
        (d_write_index < read_index)) {

        // Find reader with the smallest read index that is greater than the
        // write index
        uint32_t min_reader_index = std::numeric_limits<uint32_t>::max();
        uint32_t min_read_idx = std::numeric_limits<uint32_t>::max();
        for (size_t idx = 0; idx < d_readers.size(); ++idx) {
            if (d_readers[idx]->d_read_index > d_write_index) {
                // Record index of reader with minimum read-index
                if (d_readers[idx]->d_read_index < min_read_idx) {
                    min_read_idx = d_readers[idx]->d_read_index;
                    min_reader_index = idx;
                }
            }
        }

        // Note items_avail might be zero, that's okay.
        items_avail += read_index - min_read_idx;
        int gap = min_read_idx - d_write_index;
        if (items_avail > gap) {
            return false;
        }

#ifdef BUFFER_DEBUG
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "input_blocked_callback() WR_idx: " << d_write_index
            << " -- WR items: " << nitems_written() << " -- BUFSIZE: " << d_bufsize
            << " -- RD_idx: " << min_read_idx;
        for (size_t idx = 0; idx < d_readers.size(); ++idx) {
            if (idx != min_reader_index) {
                msg << " -- OTHER_RDR: " << d_readers[idx]->d_read_index;
            }
        }

        msg << " -- GAP: " << gap << " -- items_required: " << items_required
            << " -- items_avail: " << items_avail;
        GR_LOG_DEBUG(d_logger, msg.str());
#endif

        // Shift existing data down to make room for blocked data at end of buffer
        uint32_t move_data_size = d_write_index * d_sizeof_item;
        char* dest = buffer_ptr + (items_avail * d_sizeof_item);
        memmove_func(dest, buffer_ptr, move_data_size);

        // Next copy the data from the end of the buffer back to the beginning
        uint32_t avail_data_size = items_avail * d_sizeof_item;
        char* src = buffer_ptr + (min_read_idx * d_sizeof_item);
        memcpy_func(buffer_ptr, src, avail_data_size);

        // Now adjust write pointer
        d_write_index += items_avail;

        // Finally adjust all reader pointers
        for (size_t idx = 0; idx < d_readers.size(); ++idx) {
            if (idx == min_reader_index) {
                d_readers[idx]->d_read_index = 0;
            } else {
                d_readers[idx]->d_read_index += items_avail;
                d_readers[idx]->d_read_index %= d_bufsize;
            }
        }

        return true;
    }

    return false;
}

bool buffer_single_mapped::output_blocked_callback_logic(int output_multiple,
                                                         bool force,
                                                         char* buffer_ptr,
                                                         mem_func_t const& memmove_func)
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

    if ((((space_avail > 0) || (space_avail == 0 && d_has_history)) &&
         ((space_avail / output_multiple) * output_multiple == 0)) ||
        force) {
        // Find reader with the smallest read index
        uint32_t min_read_idx = d_readers[0]->d_read_index;
        uint64_t min_read_idx_nitems = d_readers[0]->nitems_read();
        for (size_t idx = 1; idx < d_readers.size(); ++idx) {
            // Record index of reader with minimum read-index
            if (d_readers[idx]->d_read_index < min_read_idx) {
                min_read_idx = d_readers[idx]->d_read_index;
                min_read_idx_nitems = d_readers[idx]->nitems_read();
            }
        }

#ifdef BUFFER_DEBUG
        std::ostringstream msg;
        msg << "[" << this << "] "
            << "output_blocked_callback() WR_idx: " << d_write_index
            << " -- WR items: " << nitems_written() << " -- min RD_idx: " << min_read_idx
            << " -- RD items: " << min_read_idx_nitems << " -- shortcircuit: "
            << ((min_read_idx == 0) || (min_read_idx > d_write_index) ||
                (min_read_idx == d_write_index &&
                 min_read_idx_nitems != nitems_written()))
            << " -- to_move_items: " << (d_write_index - min_read_idx)
            << " -- space_avail: " << space_avail << " -- force: " << force;
        GR_LOG_DEBUG(d_logger, msg.str());
#endif

        // Make sure we have enough room to start writing back at the beginning
        if ((min_read_idx == 0) || (min_read_idx > d_write_index) ||
            (min_read_idx == d_write_index && min_read_idx_nitems != nitems_written())) {
            return false;
        }

        // Determine how much "to be read" data needs to be moved
        int to_move_items = d_write_index - min_read_idx;
        if (to_move_items > 0) {
            uint32_t to_move_bytes = to_move_items * d_sizeof_item;

            // Shift "to be read" data back to the beginning of the buffer
            memmove_func(
                buffer_ptr, buffer_ptr + (min_read_idx * d_sizeof_item), to_move_bytes);
        }

        // Adjust write index and each reader index
        d_write_index -= min_read_idx;

        for (size_t idx = 0; idx < d_readers.size(); ++idx) {
            d_readers[idx]->d_read_index -= min_read_idx;
        }

        return true;
    }

    return false;
}

} /* namespace gr */
