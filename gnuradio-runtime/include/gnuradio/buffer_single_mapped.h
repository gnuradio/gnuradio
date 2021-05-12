/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_BUFFER_SINGLE_MAPPED_H
#define INCLUDED_GR_RUNTIME_BUFFER_SINGLE_MAPPED_H

#include <functional>

#include <gnuradio/api.h>
#include <gnuradio/block.h>
#include <gnuradio/buffer.h>
#include <gnuradio/logger.h>
#include <gnuradio/runtime_types.h>

namespace gr {

/*!
 * TODO: update this
 *
 * \brief Single writer, multiple reader fifo.
 * \ingroup internal
 */
class GR_RUNTIME_API buffer_single_mapped : public buffer
{
public:
    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;

    virtual ~buffer_single_mapped();

    /*!
     * \brief Return the block that owns this buffer.
     */
    block_sptr buf_owner() { return d_buf_owner; }

    /*!
     * \brief return number of items worth of space available for writing
     */
    virtual int space_available();

    virtual void update_reader_block_history(unsigned history, int delay)
    {
        unsigned old_max = d_max_reader_history;
        d_max_reader_history = std::max(d_max_reader_history, history);
        if (d_max_reader_history != old_max) {
            d_write_index = d_max_reader_history - 1;

#ifdef BUFFER_DEBUG
            std::ostringstream msg;
            msg << "[" << this << "] "
                << "buffer_single_mapped constructor -- set wr index to: "
                << d_write_index;
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

    void deleter(char* ptr)
    {
        // Delegate free of the underlying buffer to the block that owns it
        if (ptr != nullptr)
            buf_owner()->free_custom_buffer(ptr);
    }

    /*!
     * \brief Return true if thread is ready to call the callback, false otherwise
     */
    virtual bool output_blkd_cb_ready(int output_multiple);

    /*!
     * \brief Callback function that the scheduler will call when it determines
     * that the output is blocked
     */
    virtual bool output_blocked_callback(int output_multiple, bool force);

protected:
    /*!
     * sets d_base, d_bufsize.
     * returns true iff successful.
     */
    bool allocate_buffer(int nitems, size_t sizeof_item, uint64_t downstream_lcm_nitems);

    virtual unsigned index_add(unsigned a, unsigned b)
    {
        unsigned s = a + b;

        if (s >= d_bufsize)
            s -= d_bufsize;

        assert(s < d_bufsize);
        return s;
    }

    virtual unsigned index_sub(unsigned a, unsigned b)
    {
        // NOTE: a is writer ptr and b is read ptr
        int s = a - b;

        if (s < 0)
            s = d_bufsize - b;

        assert((unsigned)s < d_bufsize);
        return s;
    }

private:
    friend class buffer_reader;

    friend GR_RUNTIME_API buffer_sptr make_buffer(int nitems,
                                                  size_t sizeof_item,
                                                  uint64_t downstream_lcm_nitems,
                                                  block_sptr link,
                                                  block_sptr buf_owner);

    block_sptr d_buf_owner; // block that "owns" this buffer

    std::unique_ptr<char, std::function<void(char*)>> d_buffer;

    /*!
     * \brief constructor is private.  Use gr_make_buffer to create instances.
     *
     * Allocate a buffer that holds at least \p nitems of size \p sizeof_item.
     *
     * \param nitems is the minimum number of items the buffer will hold.
     * \param sizeof_item is the size of an item in bytes.
     * \param downstream_lcm_nitems is the least common multiple of the items to
     *                              read by downstream blocks
     * \param link is the block that writes to this buffer.
     * \param buf_owner if the block that owns the buffer which may or may not
     *                  be the same as the block that writes to this buffer
     *
     * The total size of the buffer will be rounded up to a system
     * dependent boundary.  This is typically the system page size, but
     * under MS windows is 64KB.
     */
    buffer_single_mapped(int nitems,
                         size_t sizeof_item,
                         uint64_t downstream_lcm_nitems,
                         block_sptr link,
                         block_sptr buf_owner);
};

} /* namespace gr */


#endif /* INCLUDED_GR_RUNTIME_BUFFER_SINGLE_MAPPED_H */
