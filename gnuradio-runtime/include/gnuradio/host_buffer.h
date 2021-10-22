/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_HOST_BUFFER_H
#define INCLUDED_HOST_BUFFER_H

#include <gnuradio/buffer_single_mapped.h>
#include <gnuradio/buffer_type.h>
#include <cstddef>

namespace gr {


class GR_RUNTIME_API host_buffer : public buffer_single_mapped
{
public:
    static void* device_memcpy(void* dest, const void* src, std::size_t count);
    static void* device_memmove(void* dest, const void* src, std::size_t count);

    static buffer_type type;

    static buffer_sptr make_buffer(int nitems,
                                   size_t sizeof_item,
                                   uint64_t downstream_lcm_nitems,
                                   uint32_t downstream_max_out_mult,
                                   block_sptr link = block_sptr(),
                                   block_sptr buf_owner = block_sptr());

    ~host_buffer() override;

    /*!
     * \brief Handles post-general_work() cleanup and data transfer
     *
     * Called directly after call to general_work() completes and
     * is used for data transfer (and perhaps other administrative
     * activities)
     *
     * \param nitems is the number of items produced by the general_work() function.
     */
    void post_work(int nitems) override;

    /*!
     * \brief Do actual buffer allocation. Inherited from buffer_single_mapped.
     */
    bool do_allocate_buffer(size_t final_nitems, size_t sizeof_item) override;

    /*!
     * \brief Return a pointer to the write buffer depending on the context
     */
    void* write_pointer() override;

    /*!
     * \brief return pointer to read buffer depending on the context
     *
     * The return value points to at least items_available() items.
     */
    const void* _read_pointer(unsigned int read_index) override;

    /*!
     * \brief Callback function that the scheduler will call when it determines
     * that the input is blocked. Override this function if needed.
     */
    bool input_blocked_callback(int items_required,
                                int items_avail,
                                unsigned read_index) override;

    /*!
     * \brief Callback function that the scheduler will call when it determines
     * that the output is blocked
     */
    bool output_blocked_callback(int output_multiple, bool force) override;

    /*!
     * \brief Creates a new host_buffer object
     *
     * \param nitems
     * \param sizeof_item
     * \param downstream_lcm_nitems
     * \param downstream_max_out_mult
     * \param link
     * \param buf_owner
     *
     * \return pointer to buffer base class
     */
    static buffer_sptr make_host_buffer(int nitems,
                                        std::size_t sizeof_item,
                                        uint64_t downstream_lcm_nitems,
                                        uint32_t downstream_max_out_mult,
                                        block_sptr link,
                                        block_sptr buf_owner);

private:
    // This is the simulated device buffer
    std::unique_ptr<char[]> d_device_buf;
    char* d_device_base;

    /*!
     * \brief constructor is private. Use the static make_host_buffer function
     * to create instances.
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
    host_buffer(int nitems,
                size_t sizeof_item,
                uint64_t downstream_lcm_nitems,
                uint32_t downstream_max_out_mult,
                block_sptr link,
                block_sptr buf_owner);
};

} // namespace gr

#endif /* INCLUDED_HOST_BUFFER_H */
