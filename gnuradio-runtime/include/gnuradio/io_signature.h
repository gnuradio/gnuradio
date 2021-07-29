/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IO_SIGNATURE_H
#define INCLUDED_IO_SIGNATURE_H

#include <functional>

#include <gnuradio/api.h>
#include <gnuradio/buffer_double_mapped.h>
#include <gnuradio/buffer_type.h>
#include <gnuradio/runtime_types.h>

namespace gr {

/*!
 * \brief i/o signature for input and output ports.
 * \brief misc
 */
class GR_RUNTIME_API io_signature
{
    int d_min_streams;
    int d_max_streams;
    std::vector<int> d_sizeof_stream_item;
    gr_vector_buffer_type d_stream_buffer_type;

    io_signature(int min_streams,
                 int max_streams,
                 const std::vector<int>& sizeof_stream_items,
                 gr_vector_buffer_type buftypes);

public:
    typedef std::shared_ptr<io_signature> sptr;

    static constexpr int IO_INFINITE = -1;

    ~io_signature();

    /*!
     * \brief Create an i/o signature
     *
     * \ingroup internal
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 ->
     * infinite) \param sizeof_stream_item  specify the size of the items in each stream
     * \param buftype  type of buffers the streams should use (defaults to standard host
     * double mapped buffer)
     */
    static sptr make(int min_streams,
                     int max_streams,
                     int sizeof_stream_item,
                     buffer_type buftype = buffer_double_mapped::type);

    /*!
     * \brief Create an i/o signature
     *
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 ->
     * infinite) \param sizeof_stream_item1 specify the size of the items in the first
     * stream \param sizeof_stream_item2 specify the size of the items in the second and
     * subsequent streams
     * \param buftype1  type of buffers the first stream should use (defaults to standard
     * host double mapped buffer)
     * \param buftype2  type of buffers the second and subsequent streams should use
     * (defaults to standard host double mapped buffer)
     */
    static sptr make2(int min_streams,
                      int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
                      buffer_type buftype1 = buffer_double_mapped::type,
                      buffer_type buftype2 = buffer_double_mapped::type);

    /*!
     * \brief Create an i/o signature
     *
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 ->
     * infinite) \param sizeof_stream_item1 specify the size of the items in the first
     * stream \param sizeof_stream_item2 specify the size of the items in the second
     * stream \param sizeof_stream_item3 specify the size of the items in the third and
     * subsequent streams
     * \param buftype1  type of buffers the first stream should use (defaults to standard
     * host double mapped buffer)
     * \param buftype2  type of buffers the second stream should use (defaults to standard
     * host double mapped buffer)
     * \param buftype3  type of buffers the third and subsequent streams should use
     * (defaults to standard host double mapped buffer)
     */
    static sptr make3(int min_streams,
                      int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
                      int sizeof_stream_item3,
                      buffer_type buftype1 = buffer_double_mapped::type,
                      buffer_type buftype2 = buffer_double_mapped::type,
                      buffer_type buftype3 = buffer_double_mapped::type);

    /*!
     * \brief Create an i/o signature
     *
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 ->
     * infinite) \param sizeof_stream_items specify the size of the items in the streams
     *
     * If there are more streams than there are entries in
     * sizeof_stream_items, the value of the last entry in
     * sizeof_stream_items is used for the missing values.
     * sizeof_stream_items must contain at least 1 entry.
     */
    static sptr
    makev(int min_streams, int max_streams, const std::vector<int>& sizeof_stream_items);

    /*!
     * \brief Create an i/o signature
     *
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 ->
     * infinite) \param sizeof_stream_items specify the size of the items in the streams
     * \param buftypes  the type of buffer each stream will should use
     *
     * If there are more streams than there are entries in
     * sizeof_stream_items, the value of the last entry in
     * sizeof_stream_items is used for the missing values.
     * sizeof_stream_items must contain at least 1 entry.
     */
    static sptr makev(int min_streams,
                      int max_streams,
                      const std::vector<int>& sizeof_stream_items,
                      gr_vector_buffer_type buftypes);

    int min_streams() const { return d_min_streams; }
    int max_streams() const { return d_max_streams; }
    int sizeof_stream_item(int index) const;
    std::vector<int> sizeof_stream_items() const;
    buffer_type stream_buffer_type(size_t index) const;
    gr_vector_buffer_type stream_buffer_types() const;
};

} /* namespace gr */

#endif /* INCLUDED_IO_SIGNATURE_H */
