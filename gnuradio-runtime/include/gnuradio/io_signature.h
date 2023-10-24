/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
 * Copyright 2023 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IO_SIGNATURE_H
#define INCLUDED_IO_SIGNATURE_H
/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <gnuradio/api.h>
#include <spdlog/fmt/fmt.h>
#include <string_view>
#include <memory>

#include <gnuradio/buffer_type.h>
// For testing purposes, force single mapped buffers to make all QA use them
// #define FORCE_SINGLE_MAPPED
#ifdef FORCE_SINGLE_MAPPED
#include <gnuradio/host_buffer.h>
#else
#include <gnuradio/buffer_double_mapped.h>
#endif


namespace gr {

/*!
 * \brief i/o signature for input and output ports.
 * \brief misc
 */
class GR_RUNTIME_API io_signature
{
    int d_min_streams;
    int d_max_streams;
    std::vector<size_t> d_sizeof_stream_item;
    gr_vector_buffer_type d_stream_buffer_type;

    io_signature(int min_streams,
                 int max_streams,
                 const std::vector<size_t>& sizeof_stream_items,
                 const gr_vector_buffer_type& buftypes);

public:
    typedef std::shared_ptr<io_signature> sptr;

#ifdef FORCE_SINGLE_MAPPED
    using default_buftype = host_buffer;
#else
    using default_buftype = buffer_double_mapped;
#endif

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
                     buffer_type buftype = default_buftype::type);


    /*!
     * \brief Create an i/o signature
     *
     * \ingroup internal
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 ->
     * infinite)
     * \param sizeof_stream_items specify the size of the items in each stream
     * \param buftypes  type of buffers the streams should use (defaults to standard host
     * double mapped buffer)
     */
    static sptr make(int min_streams,
                     int max_streams,
                     const std::vector<size_t>& sizeof_stream_items,
                     const gr::gr_vector_buffer_type& buftypes =
                         gr::gr_vector_buffer_type(1, default_buftype::type));

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
    //[[deprecated("Use generic make(min, max, {…})")]]
    static sptr make2(int min_streams,
                      int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
                      buffer_type buftype1 = default_buftype::type,
                      buffer_type buftype2 = default_buftype::type);

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
    //[[deprecated("Use generic make(min, max, {…})")]]
    static sptr make3(int min_streams,
                      int max_streams,
                      int sizeof_stream_item1,
                      int sizeof_stream_item2,
                      int sizeof_stream_item3,
                      buffer_type buftype1 = default_buftype::type,
                      buffer_type buftype2 = default_buftype::type,
                      buffer_type buftype3 = default_buftype::type);

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
    //[[deprecated("Use generic make(min, max, {…})")]]
    static sptr
    makev(int min_streams, int max_streams, const std::vector<int>& sizeof_stream_items);

    /*!
     * \brief Create an i/o signature
     *
     * \param min_streams  specify minimum number of streams (>= 0)
     * \param max_streams  specify maximum number of streams (>= min_streams or -1 ->
     * infinite)
     * \param sizeof_stream_items specify the size of the items in the streams
     * \param buftypes  the type of buffer each stream will should use
     *
     * If there are more streams than there are entries in
     * sizeof_stream_items, the value of the last entry in
     * sizeof_stream_items is used for the missing values.
     * sizeof_stream_items must contain at least 1 entry.
     */
    //[[deprecated("Use generic make(min, max, {…})")]]
    static sptr makev(int min_streams,
                      int max_streams,
                      const std::vector<int>& sizeof_stream_items,
                      const gr_vector_buffer_type& buftypes);

    int min_streams() const { return d_min_streams; }
    int max_streams() const { return d_max_streams; }
    int sizeof_stream_item(int index) const;
    // TODO: convert API to <size_t>
    std::vector<int> sizeof_stream_items() const;
    buffer_type stream_buffer_type(size_t index) const;
    gr_vector_buffer_type stream_buffer_types() const;
    friend bool operator==(const io_signature& lhs, const io_signature& rhs);
};

bool operator==(const io_signature& lhs, const io_signature& rhs);
} /* namespace gr */

template <>
struct GR_RUNTIME_API fmt::formatter<gr::io_signature> : formatter<std::string_view> {
    fmt::format_context::iterator format(const gr::io_signature& iosig,
                                         format_context& ctx) const;
};
#endif /* INCLUDED_IO_SIGNATURE_H */
