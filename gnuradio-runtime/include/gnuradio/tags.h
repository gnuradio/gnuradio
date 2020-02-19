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

namespace gr {

struct GR_RUNTIME_API tag_t {
    //! the item \p tag occurred at (as a uint64_t)
    uint64_t offset;

    //! the key of \p tag (as a PMT symbol)
    pmt::pmt_t key;

    //! the value of \p tag (as a PMT)
    pmt::pmt_t value;

    //! the source ID of \p tag (as a PMT)
    pmt::pmt_t srcid;

    //! Used by gr_buffer to mark a tagged as deleted by a specific block. You can usually
    //! ignore this.
    std::vector<long> marked_deleted;

    /*!
     * Comparison function to test which tag, \p x or \p y, came
     * first in time
     */
    static inline bool offset_compare(const tag_t& x, const tag_t& y)
    {
        return x.offset < y.offset;
    }

    inline bool operator==(const tag_t& t) const
    {
        return (t.key == key) && (t.value == value) && (t.srcid == srcid) &&
               (t.offset == offset);
    }

    tag_t()
        : offset(0),
          key(pmt::PMT_NIL),
          value(pmt::PMT_NIL),
          srcid(pmt::PMT_F) // consistent with default srcid value in block::add_item_tag
    {
    }

    tag_t(const tag_t& rhs)
        : offset(rhs.offset), key(rhs.key), value(rhs.value), srcid(rhs.srcid)
    {
    }
    tag_t& operator=(const tag_t& rhs)
    {
        if (this != &rhs) {
            offset = rhs.offset;
            key = rhs.key;
            value = rhs.value;
            srcid = rhs.srcid;
        }
        return (*this);
    }

    ~tag_t() {}
};

} /* namespace gr */

#endif /*INCLUDED_GR_TAGS_H*/
