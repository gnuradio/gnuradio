/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TAG_DEBUG_IMPL_H
#define INCLUDED_GR_TAG_DEBUG_IMPL_H

#include <gnuradio/blocks/tag_debug.h>
#include <gnuradio/thread/thread.h>
#include <stddef.h>

namespace gr {
namespace blocks {

class tag_debug_impl : public tag_debug
{
private:
    const std::string d_name;
    std::vector<tag_t> d_tags;
    bool d_display;
    pmt::pmt_t d_filter;
    gr::thread::mutex d_mutex;

public:
    tag_debug_impl(size_t sizeof_stream_item,
                   const std::string& name,
                   const std::string& key_filter = "");
    ~tag_debug_impl();

    void setup_rpc();

    std::vector<tag_t> current_tags();
    int num_tags();

    void set_display(bool d);

    void set_key_filter(const std::string& key_filter);
    std::string key_filter() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TAG_DEBUG_IMPL_H */
