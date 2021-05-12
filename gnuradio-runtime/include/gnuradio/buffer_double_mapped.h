/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_BUFFER_DOUBLE_MAPPED_H
#define INCLUDED_GR_RUNTIME_BUFFER_DOUBLE_MAPPED_H

#include <gnuradio/api.h>
#include <gnuradio/buffer.h>
#include <gnuradio/logger.h>
#include <gnuradio/runtime_types.h>

namespace gr {

class vmcircbuf;

/*!
 * \brief Note this function is only used and really intended to be used in
 *  qa_buffer.cc for the unit tests of buffer_double_mapped.
 *
 */
GR_RUNTIME_API buffer_sptr make_buffer_double_mapped(int nitems,
                                                     size_t sizeof_item,
                                                     uint64_t downstream_lcm_nitems,
                                                     block_sptr link = block_sptr());

/*!
 * \brief Single writer, multiple reader fifo.
 * \ingroup internal
 */
class GR_RUNTIME_API buffer_double_mapped : public buffer
{
public:
    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;

    virtual ~buffer_double_mapped();

    /*!
     * \brief return number of items worth of space available for writing
     */
    virtual int space_available();

protected:
    /*!
     * sets d_vmcircbuf, d_base, d_bufsize.
     * returns true iff successful.
     */
    bool allocate_buffer(int nitems, size_t sizeof_item);

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
        int s = a - b;

        if (s < 0)
            s += d_bufsize;

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
    friend GR_RUNTIME_API buffer_sptr make_buffer_double_mapped(
        int nitems, size_t sizeof_item, uint64_t downstream_lcm_nitems, block_sptr link);

    std::unique_ptr<gr::vmcircbuf> d_vmcircbuf;

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
     *
     * The total size of the buffer will be rounded up to a system
     * dependent boundary.  This is typically the system page size, but
     * under MS windows is 64KB.
     */
    buffer_double_mapped(int nitems,
                         size_t sizeof_item,
                         uint64_t downstream_lcm_nitems,
                         block_sptr link);
};

} /* namespace gr */


#endif /* INCLUDED_GR_RUNTIME_BUFFER_DOUBLE_MAPPED_H */
