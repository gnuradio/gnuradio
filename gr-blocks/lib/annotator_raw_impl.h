/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_ANNOTATOR_RAW_IMPL_H
#define INCLUDED_GR_ANNOTATOR_RAW_IMPL_H

#include <gnuradio/blocks/annotator_raw.h>
#include <gnuradio/thread/thread.h>
#include <type_traits>
#include <set>
#include <tuple>

namespace gr {
namespace blocks {

class annotator_raw_impl : public annotator_raw
{
private:
    struct tag_comparator {
        using is_transparent = std::true_type;
        //!\brief comparator over all fields in a tag, not just the offset
        constexpr bool operator()(const tag_t& l, const tag_t& r) const
        {
            return std::tie(l.offset, l.key, l.value, l.srcid) <
                   std::tie(r.offset, r.key, r.value, r.srcid);
        }
        constexpr bool operator()(const tag_t& l,
                                  const decltype(tag_t::offset)& r_offset) const
        {
            return l.offset < r_offset;
        }
        constexpr bool operator()(const decltype(tag_t::offset)& l_offset,
                                  const tag_t& r) const
        {
            return l_offset < r.offset;
        }
    };
    using tag_container = std::set<tag_t, tag_comparator>;
    tag_container d_queued_tags;
    gr::thread::mutex d_mutex;
    const size_t d_itemsize;

public:
    annotator_raw_impl(size_t sizeof_stream_item);
    ~annotator_raw_impl() override;

    // insert a tag to be added
    void add_tag(uint64_t offset, pmt::pmt_t key, pmt::pmt_t val) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ANNOTATOR_RAW_IMPL_H */
