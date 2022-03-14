/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/io_signature.h>
#include <algorithm>
#include <stdexcept>

namespace gr {

gr::io_signature::sptr io_signature::makev(int min_streams,
                                           int max_streams,
                                           const std::vector<int>& sizeof_stream_items)
{
    gr_vector_buffer_type buftypes(sizeof_stream_items.size(),
#ifdef FORCE_SINGLE_MAPPED
                                   host_buffer::type);
#else
                                   buffer_double_mapped::type);
#endif
    return gr::io_signature::sptr(
        new io_signature(min_streams, max_streams, sizeof_stream_items, buftypes));
}

gr::io_signature::sptr io_signature::makev(int min_streams,
                                           int max_streams,
                                           const std::vector<int>& sizeof_stream_items,
                                           gr_vector_buffer_type buftypes)
{
    return gr::io_signature::sptr(
        new io_signature(min_streams, max_streams, sizeof_stream_items, buftypes));
}

gr::io_signature::sptr io_signature::make(int min_streams,
                                          int max_streams,
                                          int sizeof_stream_item,
                                          buffer_type buftype)
{
    std::vector<int> sizeof_items{ sizeof_stream_item };
    gr_vector_buffer_type buftypes{ buftype };
    return io_signature::makev(min_streams, max_streams, sizeof_items, buftypes);
}

gr::io_signature::sptr io_signature::make2(int min_streams,
                                           int max_streams,
                                           int sizeof_stream_item1,
                                           int sizeof_stream_item2,
                                           buffer_type buftype1,
                                           buffer_type buftype2)
{
    std::vector<int> sizeof_items{ sizeof_stream_item1, sizeof_stream_item2 };
    gr_vector_buffer_type buftypes{ buftype1, buftype2 };
    return io_signature::makev(min_streams, max_streams, sizeof_items, buftypes);
}

gr::io_signature::sptr io_signature::make3(int min_streams,
                                           int max_streams,
                                           int sizeof_stream_item1,
                                           int sizeof_stream_item2,
                                           int sizeof_stream_item3,
                                           buffer_type buftype1,
                                           buffer_type buftype2,
                                           buffer_type buftype3)
{
    std::vector<int> sizeof_items{ sizeof_stream_item1,
                                   sizeof_stream_item2,
                                   sizeof_stream_item3 };
    gr_vector_buffer_type buftypes{ buftype1, buftype2, buftype3 };
    return io_signature::makev(min_streams, max_streams, sizeof_items, buftypes);
}

// ------------------------------------------------------------------------

io_signature::io_signature(int min_streams,
                           int max_streams,
                           const std::vector<int>& sizeof_stream_items,
                           gr_vector_buffer_type buftypes)
    : d_min_streams(min_streams),
      d_max_streams(max_streams),
      d_sizeof_stream_item(sizeof_stream_items),
      d_stream_buffer_type(buftypes)
{
    if (min_streams < 0 || (max_streams != IO_INFINITE && max_streams < min_streams))
        throw std::invalid_argument("gr::io_signature(1)");

    if (sizeof_stream_items.empty()) {
        throw std::invalid_argument("gr::io_signature(2)");
    }

    for (size_t i = 0; i < sizeof_stream_items.size(); i++) {
        if (max_streams != 0 && sizeof_stream_items[i] < 1)
            throw std::invalid_argument("gr::io_signature(3)");
    }
}

io_signature::~io_signature() {}

int io_signature::sizeof_stream_item(int _index) const
{
    if (_index < 0)
        throw std::invalid_argument("gr::io_signature::sizeof_stream_item");

    size_t index = _index;
    return d_sizeof_stream_item[std::min(index, d_sizeof_stream_item.size() - 1)];
}

std::vector<int> io_signature::sizeof_stream_items() const
{
    return d_sizeof_stream_item;
}

buffer_type io_signature::stream_buffer_type(size_t index) const
{
    return d_stream_buffer_type[std::min(index, d_stream_buffer_type.size() - 1)];
}

gr_vector_buffer_type io_signature::stream_buffer_types() const
{
    return d_stream_buffer_type;
}

} /* namespace gr */
