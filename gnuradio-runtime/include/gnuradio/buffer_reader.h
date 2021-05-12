/* -*- c++ -*- */
/*
 * Copyright 2004,2009-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_BUFFER_READER_H
#define INCLUDED_GR_RUNTIME_BUFFER_READER_H

#include <gnuradio/api.h>
#include <gnuradio/buffer.h>
#include <gnuradio/logger.h>
#include <gnuradio/runtime_types.h>
#include <gnuradio/tags.h>
#include <gnuradio/thread/thread.h>
#include <boost/weak_ptr.hpp>
#include <map>
#include <memory>

namespace gr {

class buffer_reader_sm;

/*!
 * \brief Create a new gr::buffer_reader and attach it to buffer \p buf
 * \param buf is the buffer the \p gr::buffer_reader reads from.
 * \param nzero_preload -- number of zero items to "preload" into buffer.
 * \param link is the block that reads from the buffer using this gr::buffer_reader.
 * \param delay Optional setting to declare the buffer's sample delay.
 */
GR_RUNTIME_API buffer_reader_sptr buffer_add_reader(buffer_sptr buf,
                                                    int nzero_preload,
                                                    block_sptr link = block_sptr(),
                                                    int delay = 0);

//! returns # of buffers currently allocated
GR_RUNTIME_API long buffer_ncurrently_allocated();


// ---------------------------------------------------------------------------

/*!
 * \brief How we keep track of the readers of a gr::buffer.
 * \ingroup internal
 */
class GR_RUNTIME_API buffer_reader
{
public:
#ifdef BUFFER_DEBUG
    // BUFFER DEBUG
    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;
#endif

    virtual ~buffer_reader();

    /*!
     * Declares the sample delay for this reader.
     *
     * See gr::block::declare_sample_delay for details.
     *
     * \param delay The new sample delay
     */
    void declare_sample_delay(unsigned delay);

    /*!
     * Gets the sample delay for this reader.
     *
     * See gr::block::sample_delay for details.
     */
    unsigned sample_delay() const;

    /*!
     * \brief Return number of items available for reading.
     */
    virtual int items_available(); // const

    /*!
     * \brief Return buffer this reader reads from.
     */
    buffer_sptr buffer() const { return d_buffer; }

    /*!
     * \brief Return maximum number of items that could ever be available for reading.
     * This is used as a sanity check in the scheduler to avoid looping forever.
     */
    int max_possible_items_available() const { return d_buffer->bufsize() - 1; }

    /*!
     * \brief return pointer to read buffer.
     *
     * The return value points to items_available() number of items
     */
    const void* read_pointer();

    /*
     * \brief tell buffer we read \p items from it
     */
    void update_read_pointer(int nitems);

    void set_done(bool done) { d_buffer->set_done(done); }
    bool done() const { return d_buffer->done(); }

    gr::thread::mutex* mutex() { return d_buffer->mutex(); }

    uint64_t nitems_read() const { return d_abs_read_offset; }

    void reset_nitem_counter()
    {
        d_read_index = 0;
        d_abs_read_offset = 0;
    }

    size_t get_sizeof_item() { return d_buffer->get_sizeof_item(); }

    /*!
     * \brief Return the block that reads via this reader.
     *
     */
    block_sptr link() { return block_sptr(d_link); }

    /*!
     * \brief Given a [start,end), returns a vector all tags in the range.
     *
     * Get a vector of tags in given range. Range of counts is from start to end-1.
     *
     * Tags are tuples of:
     *      (item count, source id, key, value)
     *
     * \param v            a vector reference to return tags into
     * \param abs_start    a uint64 count of the start of the range of interest
     * \param abs_end      a uint64 count of the end of the range of interest
     * \param id           the unique ID of the block to make sure already deleted tags
     * are not returned
     */
    void get_tags_in_range(std::vector<tag_t>& v,
                           uint64_t abs_start,
                           uint64_t abs_end,
                           long id);

    /*!
     * \brief Returns true when the current thread is ready to call the callback,
     * false otherwise. Note if input_blocked_callback is overridden then this
     * function should also be overridden.
     */
    virtual bool input_blkd_cb_ready(int items_required) const { return false; }

    /*!
     * \brief Callback function that the scheduler will call when it determines
     * that the input is blocked. Override this function if needed.
     */
    virtual bool input_blocked_callback(int items_required, int items_avail)
    {
        return false;
    }

    // -------------------------------------------------------------------------
    unsigned int get_read_index() const { return d_read_index; }
    uint64_t get_abs_read_offset() const { return d_abs_read_offset; }

protected:
    friend class buffer;
    friend class buffer_double_mapped;
    friend class buffer_single_mapped;
    friend class buffer_reader_sm;

    friend GR_RUNTIME_API buffer_reader_sptr buffer_add_reader(buffer_sptr buf,
                                                               int nzero_preload,
                                                               block_sptr link,
                                                               int delay);

    buffer_sptr d_buffer;
    unsigned int d_read_index;   // in items [0,d->buffer.d_bufsize) ** see NB
    uint64_t d_abs_read_offset;  // num items seen since the start   ** see NB
    std::weak_ptr<block> d_link; // block that reads via this buffer reader
    unsigned d_attr_delay;       // sample delay attribute for tag propagation
    // ** NB: buffer::d_mutex protects d_read_index and d_abs_read_offset

    //! constructor is private.  Use gr::buffer::add_reader to create instances
    buffer_reader(buffer_sptr buffer, unsigned int read_index, block_sptr link);
};

//! returns # of buffer_readers currently allocated
GR_RUNTIME_API long buffer_reader_ncurrently_allocated();

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_BUFFER_READER_H */
