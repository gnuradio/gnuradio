/* -*- c++ -*- */
/*
 * Copyright 2004,2009-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_BUFFER_H
#define INCLUDED_GR_RUNTIME_BUFFER_H

#include <gnuradio/api.h>
#include <gnuradio/custom_lock.h>
#include <gnuradio/logger.h>
#include <gnuradio/runtime_types.h>
#include <gnuradio/tags.h>
#include <gnuradio/thread/thread.h>
#include <gnuradio/transfer_type.h>

#include <functional>
#include <iostream>
#include <map>
#include <memory>


namespace gr {

class vmcircbuf;
class buffer_reader;
class buffer_reader_sm;

enum class buffer_mapping_type { double_mapped, single_mapped };

typedef std::function<void*(void*, const void*, std::size_t)> mem_func_t;

/*!
 * \brief Allocate a buffer that holds at least \p nitems of size \p sizeof_item.
 *
 * The total size of the buffer will be rounded up to a system
 * dependent boundary. This is typically the system page size, but
 * under MS windows is 64KB.
 *
 * \param nitems is the minimum number of items the buffer will hold.
 * \param sizeof_item is the size of an item in bytes.
 * \param downstream_lcm_nitems is the least common multiple of the items to
 *                              read by downstream block(s)
 * \param downstream_max_out_mult is the maximum output multiple of all
 *                                downstream blocks
 * \param link is the block that writes to this buffer.
 * \param buf_owner is the block that owns the buffer which may or may not
 *                     be the same as the block that writes to this buffer
 */
GR_RUNTIME_API buffer_sptr make_buffer(int nitems,
                                       size_t sizeof_item,
                                       uint64_t downstream_lcm_nitems,
                                       uint32_t downstream_max_out_mult,
                                       block_sptr link = block_sptr(),
                                       block_sptr buf_owner = block_sptr());

/*!
 * \brief Single writer, multiple reader fifo.
 * \ingroup internal
 */
class GR_RUNTIME_API buffer : public custom_lock_if
{
public:
    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;

    ~buffer() override;

    /*!
     * \brief return the buffer's mapping type
     */
    buffer_mapping_type get_mapping_type() { return d_buf_map_type; }

    /*!
     * \brief return number of items worth of space available for writing
     */
    virtual int space_available() = 0;

    /*!
     * \brief return size of this buffer in items
     */
    unsigned int bufsize() const { return d_bufsize; }

    /*!
     * \brief return the base address of the buffer
     */
    const char* base() const { return static_cast<const char*>(d_base); }

    /*!
     * \brief return pointer to write buffer.
     *
     * The return value points at space that can hold at least
     * space_available() items.
     */
    virtual void* write_pointer();

    /*!
     * \brief return pointer to read buffer.
     *
     * The return value points to at least items_available() items.
     */
    virtual const void* _read_pointer(unsigned int read_index);

    /*!
     * \brief tell buffer that we wrote \p nitems into it
     */
    void update_write_pointer(int nitems);

    void set_done(bool done);
    bool done() const { return d_done; }

    /*!
     * \brief Return the block that writes to this buffer.
     */
    block_sptr link() { return block_sptr(d_link); }

    size_t nreaders() const { return d_readers.size(); }
    buffer_reader* reader(size_t index) { return d_readers[index]; }

    gr::thread::mutex* mutex() { return &d_mutex; }

    uint64_t nitems_written() { return d_abs_write_offset; }

    void reset_nitem_counter()
    {
        d_write_index = 0;
        d_abs_write_offset = 0;
    }

    size_t get_sizeof_item() { return d_sizeof_item; }

    uint64_t get_downstream_lcm_nitems() { return d_downstream_lcm_nitems; }

    uint32_t get_max_reader_output_multiple() { return d_max_reader_output_multiple; }

    virtual void update_reader_block_history(unsigned history, [[maybe_unused]] int delay)
    {
        d_max_reader_history = std::max(d_max_reader_history, history);
        d_has_history = (d_max_reader_history > 1);
    }

    /*!
     * \brief  Adds a new tag to the buffer.
     *
     * \param tag        the new tag
     */
    void add_item_tag(const tag_t& tag);

    /*!
     * \brief  Removes all tags before \p max_time from buffer
     *
     * \param max_time        the time (item number) to trim up until.
     */
    void prune_tags(uint64_t max_time);

    std::multimap<uint64_t, tag_t>::iterator get_tags_begin()
    {
        return d_item_tags.begin();
    }
    std::multimap<uint64_t, tag_t>::iterator get_tags_end() { return d_item_tags.end(); }
    std::multimap<uint64_t, tag_t>::iterator get_tags_lower_bound(uint64_t x)
    {
        return d_item_tags.lower_bound(x);
    }
    std::multimap<uint64_t, tag_t>::iterator get_tags_upper_bound(uint64_t x)
    {
        return d_item_tags.upper_bound(x);
    }

    /*!
     * \brief Function to be executed after this object's owner completes the
     * call to general_work()
     */
    virtual void post_work(int nitems) = 0;

    /*!
     * \brief Returns true when the current thread is ready to call the callback,
     * false otherwise. Note if input_blocked_callback is overridden then this
     * function should also be overridden.
     */
    virtual bool input_blkd_cb_ready([[maybe_unused]] int items_required,
                                     [[maybe_unused]] unsigned read_index)
    {
        return false;
    }

    /*!
     * \brief Callback function that the scheduler will call when it determines
     * that the input is blocked. Override this function if needed.
     */
    virtual bool input_blocked_callback([[maybe_unused]] int items_required,
                                        [[maybe_unused]] int items_avail,
                                        [[maybe_unused]] unsigned read_index)
    {
        return false;
    }

    /*!
     * \brief Returns true if the current thread is ready to execute
     * output_blocked_callback(), false otherwise. Note if the default
     * output_blocked_callback is overridden this function should also be
     * overridden.
     */
    virtual bool output_blkd_cb_ready([[maybe_unused]] int output_multiple)
    {
        return false;
    }

    /*!
     * \brief Callback function that the scheduler will call when it determines
     * that the output is blocked. Override this function if needed.
     */
    virtual bool output_blocked_callback([[maybe_unused]] int output_multiple,
                                         [[maybe_unused]] bool force = false)
    {
        return false;
    }

    /*!
     * \brief Increment the number of active pointers for this buffer.
     */
    inline void increment_active()
    {
        gr::thread::scoped_lock lock(d_mutex);

        d_cv.wait(lock, [this]() { return d_callback_flag == false; });
        ++d_active_pointer_counter;
    }

    /*!
     * \brief Decrement the number of active pointers for this buffer and signal
     * anyone waiting when the count reaches zero.
     */
    inline void decrement_active()
    {
        gr::thread::scoped_lock lock(d_mutex);

        if (--d_active_pointer_counter == 0)
            d_cv.notify_all();
    }

    /*!
     * \brief "on_lock" function from the custom_lock_if.
     */
    void on_lock(gr::thread::scoped_lock& lock) override;

    /*!
     * \brief "on_unlock" function from the custom_lock_if.
     */
    void on_unlock() override;

    friend std::ostream& operator<<(std::ostream& os, const buffer& buf);

    // -------------------------------------------------------------------------

    /*!
     * \brief Assign buffer's transfer_type
     */
    void set_transfer_type(const transfer_type& type);

private:
    friend class buffer_reader;
    friend class buffer_reader_sm;

    friend GR_RUNTIME_API buffer_sptr make_buffer(int nitems,
                                                  size_t sizeof_item,
                                                  uint64_t downstream_lcm_nitems,
                                                  block_sptr link);
    friend GR_RUNTIME_API buffer_reader_sptr buffer_add_reader(buffer_sptr buf,
                                                               int nzero_preload,
                                                               block_sptr link,
                                                               int delay);

protected:
    char* d_base;           // base address of buffer inside d_vmcircbuf.
    unsigned int d_bufsize; // in items
    buffer_mapping_type d_buf_map_type;

    // Keep track of maximum sample delay of any reader; Only prune tags past this.
    unsigned d_max_reader_delay;

    // Keep track of the maximum sample history requirements of all blocks that
    // consume from this buffer
    unsigned d_max_reader_history;

    // Indicates if d_max_reader_history > 1
    bool d_has_history;

    size_t d_sizeof_item; // in bytes
    std::vector<buffer_reader*> d_readers;
    std::weak_ptr<block> d_link; // block that writes to this buffer

    //
    // The mutex protects d_write_index, d_abs_write_offset, d_done, d_item_tags
    // and the d_read_index's and d_abs_read_offset's in the buffer readers.
    // Also d_callback_flag and d_active_pointer_counter.
    //
    gr::thread::mutex d_mutex;
    unsigned int d_write_index;  // in items [0,d_bufsize)
    uint64_t d_abs_write_offset; // num items written since the start
    bool d_done;
    std::multimap<uint64_t, tag_t> d_item_tags;
    uint64_t d_last_min_items_read;
    //
    gr::thread::condition_variable d_cv;
    bool d_callback_flag;
    uint32_t d_active_pointer_counter;

    uint64_t d_downstream_lcm_nitems;
    uint64_t d_write_multiple;
    uint32_t d_max_reader_output_multiple;

    transfer_type d_transfer_type;

    /*!
     * \brief  Increment read or write index for this buffer
     */
    virtual unsigned index_add(unsigned a, unsigned b) = 0;

    /*!
     * \brief  Decrement read or write index for this buffer
     */
    virtual unsigned index_sub(unsigned a, unsigned b) = 0;

    virtual bool allocate_buffer([[maybe_unused]] int nitems) { return false; };

    /*!
     * \brief constructor is private.  Use gr_make_buffer to create instances.
     *
     * Allocate a buffer that holds at least \p nitems of size \p sizeof_item.
     *
     * \param buftype is an enum type that describes the buffer TODO: fix me
     * \param nitems is the minimum number of items the buffer will hold.
     * \param sizeof_item is the size of an item in bytes.
     * \param downstream_lcm_nitems is the least common multiple of the items to
     *                              read by downstream block(s)
     * \param downstream_max_out_mult is the maximum output multiple of all
     *                                downstream blocks
     * \param link is the block that writes to this buffer.
     *
     * The total size of the buffer will be rounded up to a system
     * dependent boundary.  This is typically the system page size, but
     * under MS windows is 64KB.
     */
    buffer(buffer_mapping_type buftype,
           int nitems,
           size_t sizeof_item,
           uint64_t downstream_lcm_nitems,
           uint32_t downstream_max_out_mult,
           block_sptr link);

    /*!
     * \brief disassociate \p reader from this buffer
     */
    void drop_reader(buffer_reader* reader);
};

//! returns # of buffers currently allocated
GR_RUNTIME_API long buffer_ncurrently_allocated();

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_BUFFER_H */
