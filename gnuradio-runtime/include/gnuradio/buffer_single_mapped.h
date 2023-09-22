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

#include <cstddef>

#include <gnuradio/api.h>
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/logger.h>
#include <gnuradio/runtime_types.h>

namespace gr {

/*!
 * \brief A single mapped buffer where wrapping conditions are handled explicitly
 * via input/output_blocked_callback functions called from block_executor.
 * \ingroup internal
 */
class GR_RUNTIME_API buffer_single_mapped : public buffer
{
public:
    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;

    ~buffer_single_mapped() override;

    /*!
     * \brief Return the block that owns this buffer.
     */
    block_sptr buf_owner() { return d_buf_owner; }

    /*!
     * \brief return number of items worth of space available for writing
     */
    int space_available() override;

    void update_reader_block_history(unsigned history, int delay) override;

    /*!
     * \brief Return true if thread is ready to call input_blocked_callback,
     * false otherwise
     */
    bool input_blkd_cb_ready(int items_required, unsigned read_index) override;

    /*!
     * \brief Callback function that the scheduler will call when it determines
     * that the input is blocked. Override this function if needed.
     */
    bool input_blocked_callback(int items_required,
                                int items_avail,
                                unsigned read_index) override = 0;

    /*!
     * \brief Return true if thread is ready to call the callback, false otherwise
     */
    bool output_blkd_cb_ready(int output_multiple) override;

    /*!
     * \brief Callback function that the scheduler will call when it determines
     * that the output is blocked
     */
    bool output_blocked_callback(int output_multiple, bool force) override = 0;

protected:
    /*!
     * \brief Make reasonable attempt to adjust nitems based on read/write
     * granularity then delegate actual allocation to do_allocate_buffer().
     * @return true iff successful.
     */
    bool allocate_buffer(int nitems) override;

    /*!
     * \brief Do actual buffer allocation. This is intended (required) to be
     * handled by the derived class.
     */
    virtual bool do_allocate_buffer(size_t final_nitems, size_t sizeof_item) = 0;

    unsigned index_add(unsigned a, unsigned b) override
    {
        unsigned s = a + b;

        if (s >= d_bufsize)
            s -= d_bufsize;

        assert(s < d_bufsize);
        return s;
    }

    unsigned index_sub(unsigned a, unsigned b) override
    {
        // NOTE: a is writer ptr and b is read ptr
        int s = a - b;

        if (s < 0)
            s = d_bufsize - b;

        assert((unsigned)s < d_bufsize);
        return s;
    }


    friend class buffer_reader;

    friend GR_RUNTIME_API buffer_sptr make_buffer(int nitems,
                                                  size_t sizeof_item,
                                                  uint64_t downstream_lcm_nitems,
                                                  block_sptr link,
                                                  block_sptr buf_owner);

    block_sptr d_buf_owner; // block that "owns" this buffer

    std::unique_ptr<char[]> d_buffer;

    /*!
     * \brief constructor is private.  Use gr_make_buffer to create instances.
     *
     * Allocate a buffer that holds at least \p nitems of size \p sizeof_item.
     *
     * \param nitems is the minimum number of items the buffer will hold.
     * \param sizeof_item is the size of an item in bytes.
     * \param downstream_lcm_nitems is the least common multiple of the items to
     *                              read by downstream blocks
     * \param downstream_max_out_mult is the maximum output multiple of all
     *                                downstream blocks
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
                         uint32_t downstream_max_out_mult,
                         block_sptr link,
                         block_sptr buf_owner);

    /*!
     * \brief Abstracted logic for the input blocked callback function.
     *
     * This function contains the logic for the input blocked callback however
     * the data adjustment portion of the callback has been abstracted to allow
     * the caller to pass in the desired buffer and corresponding buffer
     * manipulation functions (memcpy and memmove).
     *
     * The input blocked callback is called when a reader needs to read more
     * data than is available in a buffer and the available data is located at
     * the end of the buffer. The input blocked callback will attempt to move
     * any data located at the beginning of the buffer "down", and will then
     * attempt to copy from the end of the buffer back to the beginning of the
     * buffer. This process explicitly handles wrapping for a single mapped
     * buffer and will realign the data at the beginning of the buffer such
     * that the reader is able to read the available data and becomes unblocked.
     *
     * \param items_required is the number of items required by the reader
     * \param items_avail is the number of items available
     * \param read_index is the current read index of the buffer reader caller
     * \param buffer_ptr is the pointer to the desired buffer
     * \param memcpy_func is a pointer to a memcpy function appropriate for the
     *                    the passed in buffer
     * \param memmove_func is a pointer to a memmove function appropriate for
     *                     the passed in buffer
     */
    virtual bool input_blocked_callback_logic(int items_required,
                                              int items_avail,
                                              unsigned read_index,
                                              char* buffer_ptr,
                                              mem_func_t const& memcpy_func,
                                              mem_func_t const& memmove_func);

    /*!
     * \brief Abstracted logic for the output blocked callback function.
     *
     * This function contains the logic for the output blocked callback however
     * the data adjustment portion of the callback has been abstracted to allow
     * the caller to pass in the desired buffer and corresponding buffer
     * manipulation functions (memcpy and memmove).
     *
     * The output blocked callback is called when a block needs to write data
     * to the end of a single mapped buffer but not enough free space exists to
     * write the data before the end of the buffer is reached. The output blocked
     * callback will attempt to copy data located towards the end of a single
     * mapped buffer back to the beginning of the buffer. This process explicitly
     * handles wrapping for a single mapped buffer and will realign data located
     * at the end of a buffer back to the beginning of the buffer such that the
     * writing block can write its output into the buffer after the existing data.
     *
     * \param output_multiple
     * \param force run the callback disregarding the internal checks
     * \param buffer_ptr is the pointer to the desired buffer
     * \param memmove_func is a pointer to a memmove function appropriate for
     *                     the passed in buffer
     */
    virtual bool output_blocked_callback_logic(int output_multiple,
                                               bool force,
                                               char* buffer_ptr,
                                               mem_func_t const& memmove_func);
};

} /* namespace gr */


#endif /* INCLUDED_GR_RUNTIME_BUFFER_SINGLE_MAPPED_H */
