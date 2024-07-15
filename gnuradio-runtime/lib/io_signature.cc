/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2013 Free Software Foundation, Inc.
 * Copyright 2023 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/io_signature.h>

/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <spdlog/fmt/fmt.h>
#if __has_include(<spdlog/fmt/ranges.h>)
#include <spdlog/fmt/ranges.h>
#elif __has_include(<spdlog/fmt/bundled/ranges.h>)
#include <spdlog/fmt/bundled/ranges.h>
#endif
#include <string_view>
#include <algorithm>
#include <memory>
#include <stdexcept>

namespace gr {
// The one we actually implement, the rest is just convenience wrappers
gr::io_signature::sptr io_signature::make(int min_streams,
                                          int max_streams,
                                          const std::vector<size_t>& sizeof_stream_items,
                                          const gr::gr_vector_buffer_type& buftypes)
{
    return sptr(
        new io_signature(min_streams, max_streams, sizeof_stream_items, buftypes));
}

// keep as <int> for backwards compatibility
gr::io_signature::sptr io_signature::makev(int min_streams,
                                           int max_streams,
                                           const std::vector<int>& sizeof_stream_items)
{
    return io_signature::make(
        min_streams,
        max_streams,
        std::vector<size_t>(sizeof_stream_items.begin(), sizeof_stream_items.end()),
        gr_vector_buffer_type(sizeof_stream_items.size(), default_buftype::type));
}

// keep as <int> for backwards compatibility
gr::io_signature::sptr io_signature::makev(int min_streams,
                                           int max_streams,
                                           const std::vector<int>& sizeof_stream_items,
                                           const gr_vector_buffer_type& buftypes)
{
    return io_signature::make(
        min_streams,
        max_streams,
        std::vector<size_t>(sizeof_stream_items.begin(), sizeof_stream_items.end()),
        buftypes);
}

// keep as <int> for backwards compatibility
gr::io_signature::sptr io_signature::make(int min_streams,
                                          int max_streams,
                                          int sizeof_stream_item,
                                          buffer_type buftype)
{
    if (max_streams && sizeof_stream_item < 1) {
        throw std::invalid_argument(
            fmt::format("Item sizes need to be >= 1, but got {:d}", sizeof_stream_item));
    }
    return io_signature::make(
        min_streams,
        max_streams,
        std::vector<size_t>{ static_cast<size_t>(sizeof_stream_item) },
        gr::gr_vector_buffer_type{ buftype });
}

// keep as <int> for backwards compatibility
gr::io_signature::sptr io_signature::make2(int min_streams,
                                           int max_streams,
                                           int sizeof_stream_item1,
                                           int sizeof_stream_item2,
                                           buffer_type buftype1,
                                           buffer_type buftype2)
{
    return io_signature::make(
        min_streams,
        max_streams,
        std::vector<size_t>({ static_cast<size_t>(sizeof_stream_item1),
                              static_cast<size_t>(sizeof_stream_item2) }),
        { buftype1, buftype2 });
}

// keep as <int> for backwards compatibility
gr::io_signature::sptr io_signature::make3(int min_streams,
                                           int max_streams,
                                           int sizeof_stream_item1,
                                           int sizeof_stream_item2,
                                           int sizeof_stream_item3,
                                           buffer_type buftype1,
                                           buffer_type buftype2,
                                           buffer_type buftype3)
{
    return io_signature::make(
        min_streams,
        max_streams,
        std::vector<size_t>{ static_cast<size_t>(sizeof_stream_item1),
                             static_cast<size_t>(sizeof_stream_item2),
                             static_cast<size_t>(sizeof_stream_item3) },
        { buftype1, buftype2, buftype3 });
}

// ------------------------------------------------------------------------
template <class container>
auto subrange_helper(int max,
                     const container& cont,
                     std::string_view what = "sizeof_stream_items")
{
    if (max == io_signature::IO_INFINITE) {
        return cont.end();
    }
    if (max < 0) {
        throw std::invalid_argument(fmt::format("Negative maximum count {:d}", max));
    }
    if (static_cast<size_t>(max) < cont.size()) {
        return cont.begin() + max;
    }
    return cont.end();
}
io_signature::io_signature(int min_streams,
                           int max_streams,
                           const std::vector<size_t>& sizeof_stream_items,
                           const gr_vector_buffer_type& buftypes)
    : d_min_streams(min_streams),
      d_max_streams(max_streams),
      d_sizeof_stream_item(sizeof_stream_items.begin(),
                           subrange_helper(max_streams, sizeof_stream_items)),
      d_stream_buffer_type(buftypes)
{
    if (min_streams < 0) {
        throw std::invalid_argument(
            fmt::format("Minimum number of streams must be >= 0, is {:d}", min_streams));
    }
    if (max_streams != IO_INFINITE && max_streams < min_streams) {
        throw std::invalid_argument(
            fmt::format("Maximum number of streams ({:d}) needs to be at least as "
                        "large as minimum number of streams ({:d})",
                        max_streams,
                        min_streams));
    }
    if (sizeof_stream_items.empty()) {
        throw std::invalid_argument("Item size vector cannot be empty");
    }
    if (max_streams) {
        size_t index = 0;
        for (const auto size : sizeof_stream_items) {
            if (size < 1) {
                throw std::invalid_argument(fmt::format(
                    "No item size can be < 1, but position [{:d}] is {:d}", index, size));
            }
            ++index;
        }
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
    return { d_sizeof_stream_item.begin(), d_sizeof_stream_item.end() };
}

buffer_type io_signature::stream_buffer_type(size_t index) const
{
    return d_stream_buffer_type[std::min(index, d_stream_buffer_type.size() - 1)];
}

gr_vector_buffer_type io_signature::stream_buffer_types() const
{
    return d_stream_buffer_type;
}

bool operator==(const io_signature& lhs, const io_signature& rhs)
{
    return lhs.d_min_streams == rhs.d_max_streams &&
           lhs.d_max_streams == rhs.d_max_streams &&
           lhs.d_sizeof_stream_item == rhs.d_sizeof_stream_item;
    // TODO: implement buffer_type comparator
    //&&lhs.d_stream_buffer_type == rhs.d_stream_buffer_type;
}
} /* namespace gr */

fmt::format_context::iterator
fmt::formatter<gr::io_signature>::format(const gr::io_signature& iosig,
                                         fmt::format_context& ctx) const
{
    auto min_streams = iosig.min_streams();
    auto max_streams = iosig.max_streams();
    const auto itemsizes = iosig.sizeof_stream_items();

    const auto stream_range = (max_streams == gr::io_signature::IO_INFINITE)
                                  ? fmt::format("{} – {}", min_streams, max_streams)
                                  : fmt::format("{} – ∞", min_streams);

    const auto sizes = (itemsizes.size() < static_cast<size_t>(max_streams))
                           ? fmt::format("{}, …", fmt::join(itemsizes, ", "))
                           : fmt::format("{}", fmt::join(itemsizes, ", "));

    return fmt::format_to(ctx.out(),
                          "Stream Range: {:>20s}\n"
                          "Item Sizes:   {:>20s}\n"
                          "Buffer Types: {:>20s}",
                          (max_streams == gr::io_signature::IO_INFINITE)
                              ? fmt::format("{} – {}", min_streams, max_streams)
                              : fmt::format("{} – ∞", min_streams),
                          (itemsizes.size() < static_cast<size_t>(max_streams))
                              ? fmt::format("{}, …", fmt::join(itemsizes, ", "))
                              : fmt::format("{}", fmt::join(itemsizes, ", ")),
                          // TODO: Implement printing of buffer types
                          "printing not implemented");
}
