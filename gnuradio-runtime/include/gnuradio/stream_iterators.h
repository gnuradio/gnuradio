/* -*- c++ -*- */
/*
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: (GPL-3.0-or-later or LGPL-3.0-or-later)
 *
 */
#pragma once
#include <gnuradio/api.h>
#include <type_traits>
namespace gr {
template <typename ptr_t>
struct GR_RUNTIME_API item_span {
    using pointer_type = ptr_t;
    using element_type = std::remove_pointer<pointer_type>;
    pointer_type items;
    std::size_t length;
    inline pointer_type data() const { return items; }
    inline pointer_type begin() const { return items; }
    inline pointer_type end() const { return items + length; }
    auto& operator[](std::size_t n) { return data()[n]; }
};

template <typename pointer_to_item_t>
struct GR_RUNTIME_API stream_span {
    using element_type = pointer_to_item_t*;

    element_type first_item_ptr;
    std::size_t length;
    std::size_t nitems;
    class stream_iter
    {
    private:
        element_type current;
        const std::size_t nitems;

    public:
        stream_iter(element_type start, std::size_t nitems)
            : current(start), nitems(nitems)
        {
        }
        inline stream_iter operator++(int) { return stream_iter(current++, nitems); }
        inline stream_iter& operator++()
        {
            ++current;
            return *this;
        }
        inline bool operator==(const stream_iter& other) const
        {
            return current == other.current;
        }
        inline bool operator!=(const stream_iter& other) const
        {
            return current != other.current;
        }
        inline item_span<std::remove_pointer_t<element_type>> operator*() const
        {
            return { *current, nitems };
        }
    };

    inline element_type data() const { return first_item_ptr; }
    inline std::size_t size() const { return length; }
    inline stream_iter begin() const { return { first_item_ptr, nitems }; }
    inline stream_iter end() const { return { first_item_ptr + length, nitems }; }
    auto& operator[](std::size_t n) { return *(data() + n); }
    stream_span(element_type first, std::size_t length, std::size_t nitems)
        : first_item_ptr(first), length(length), nitems(nitems)
    {
    }
};
} // namespace gr
