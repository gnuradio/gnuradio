/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TAGS_H
#define INCLUDED_GR_TAGS_H

#include <gnuradio/api.h>
#include <pmt/pmt.h>

/* ensure that tweakme.h is included before the bundled spdlog/fmt header, see
 * https://github.com/gabime/spdlog/issues/2922 */
#include <spdlog/tweakme.h>

#include <gnuradio/api.h>
#include <pmt/pmt.h>
#include <spdlog/fmt/fmt.h>
#include <string_view>

namespace gr {

struct GR_RUNTIME_API tag_t {
    //! the item \p tag occurred at (as a uint64_t)
    uint64_t offset = 0;

    //! the key of \p tag (as a PMT symbol)
    pmt::pmt_t key = pmt::PMT_NIL;

    //! the value of \p tag (as a PMT)
    pmt::pmt_t value = pmt::PMT_NIL;

    //! the source ID of \p tag (as a PMT)
    pmt::pmt_t srcid = pmt::PMT_F;

    //! Comparison function to test which tag, \p x or \p y, came first in time
    friend inline bool operator<(const tag_t& x, const tag_t& y)
    {
        return x.offset < y.offset;
    }
    //! Comparison function to test which tag, \p x or \p y, came first in time
    static inline bool offset_compare(const tag_t& x, const tag_t& y) { return x < y; }

    //! equality comparison. Compares all details, except marked_delete
    inline bool operator==(const tag_t& t) const
    {
        return (t.key == key) && (t.value == value) && (t.srcid == srcid) &&
               (t.offset == offset);
    }
};

} /* namespace gr */

//!\brief enables gr::tag to be formatted with fmt
template <>
struct GR_RUNTIME_API fmt::formatter<gr::tag_t> : formatter<std::string_view> {
    fmt::format_context::iterator format(const gr::tag_t& tag, format_context& ctx) const;
};
#endif /*INCLUDED_GR_TAGS_H*/
