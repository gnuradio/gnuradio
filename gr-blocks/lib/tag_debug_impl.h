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
#include <cstddef>

namespace gr {
namespace blocks {

class tag_debug_impl : public tag_debug
{
private:
    const std::string d_name;
    std::vector<tag_t> d_work_tags; /** tags from last work */
    std::vector<tag_t> d_tags;      /** optionally holds all tags ever received */
    bool d_display;
    bool d_save_all;
    pmt::pmt_t d_filter;
    gr::thread::mutex d_mutex;

public:
    tag_debug_impl(size_t sizeof_stream_item,
                   const std::string& name,
                   const std::string& key_filter = "");
    ~tag_debug_impl() override;

    void setup_rpc() override;

    std::vector<tag_t> current_tags() override;
    int num_tags() override;

    void set_display(bool d) override;

    void set_save_all(bool s) override;

    void set_key_filter(const std::string& key_filter) override;
    std::string key_filter() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TAG_DEBUG_IMPL_H */
