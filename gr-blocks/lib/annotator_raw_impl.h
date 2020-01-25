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

namespace gr {
namespace blocks {

class annotator_raw_impl : public annotator_raw
{
private:
    size_t d_itemsize;
    std::vector<tag_t> d_queued_tags;
    gr::thread::mutex d_mutex;

public:
    annotator_raw_impl(size_t sizeof_stream_item);
    ~annotator_raw_impl();

    // insert a tag to be added
    void add_tag(uint64_t offset, pmt::pmt_t key, pmt::pmt_t val);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_ANNOTATOR_RAW_IMPL_H */
