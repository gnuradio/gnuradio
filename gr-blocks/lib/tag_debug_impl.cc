/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tag_debug_impl.h"
#include <gnuradio/io_signature.h>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace gr {
namespace blocks {

tag_debug::sptr tag_debug::make(size_t sizeof_stream_item,
                                const std::string& name,
                                const std::string& key_filter)
{
    return gnuradio::make_block_sptr<tag_debug_impl>(
        sizeof_stream_item, name, key_filter);
}

tag_debug_impl::tag_debug_impl(size_t sizeof_stream_item,
                               const std::string& name,
                               const std::string& key_filter)
    : sync_block("tag_debug",
                 io_signature::make(1, -1, sizeof_stream_item),
                 io_signature::make(0, 0, 0)),
      d_name(name),
      d_display(true),
      d_save_all(false)
{
    set_key_filter(key_filter);
}

tag_debug_impl::~tag_debug_impl() {}

std::vector<tag_t> tag_debug_impl::current_tags()
{
    gr::thread::scoped_lock l(d_mutex);

    if (d_save_all) {
        return d_tags;
    } else {
        return d_work_tags;
    }
}

int tag_debug_impl::num_tags()
{
    std::vector<tag_t> t;
    get_tags_in_range(t, 0, 0, nitems_read(0));
    return static_cast<int>(t.size());
}

void tag_debug_impl::set_display(bool d) { d_display = d; }

void tag_debug_impl::set_save_all(bool s)
{
    gr::thread::scoped_lock l(d_mutex);

    if (d_save_all && !s) {
        d_tags.clear();
    }

    d_save_all = s;
}

void tag_debug_impl::set_key_filter(const std::string& key_filter)
{
    if (key_filter.empty())
        d_filter = pmt::PMT_NIL;
    else
        d_filter = pmt::intern(key_filter);
}

std::string tag_debug_impl::key_filter() const { return pmt::symbol_to_string(d_filter); }

int tag_debug_impl::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(d_mutex);
    bool toprint = false;

    std::stringstream sout;
    if (d_display) {
        sout << std::endl
             << "----------------------------------------------------------------------";
        sout << std::endl << "Tag Debug: " << d_name << std::endl;
    }

    uint64_t abs_N, end_N;
    for (size_t i = 0; i < input_items.size(); i++) {
        abs_N = nitems_read(i);
        end_N = abs_N + (uint64_t)(noutput_items);

        d_work_tags.clear();
        if (pmt::is_null(d_filter))
            get_tags_in_range(d_work_tags, i, abs_N, end_N);
        else
            get_tags_in_range(d_work_tags, i, abs_N, end_N, d_filter);

        if (!d_work_tags.empty())
            toprint = true;

        if (d_display) {
            sout << "Input Stream: " << std::setw(2) << std::setfill('0') << i
                 << std::setfill(' ') << std::endl;
            for (const auto& tag : d_work_tags) {
                sout << std::setw(10) << "Offset: " << tag.offset << std::setw(10)
                     << "Source: "
                     << (pmt::is_symbol(tag.srcid) ? pmt::symbol_to_string(tag.srcid)
                                                   : "n/a")
                     << std::setw(10) << "Key: " << pmt::symbol_to_string(tag.key)
                     << std::setw(10) << "Value: ";
                sout << tag.value << std::endl;
            }
        }

        if (d_save_all) {
            for (const auto& tag : d_work_tags)
                d_tags.push_back(tag);
        }
    }

    if (d_display) {
        sout << "----------------------------------------------------------------------";
        sout << std::endl;

        if (toprint) {
            std::cout << sout.str();
        }
    }

    return noutput_items;
}

void tag_debug_impl::setup_rpc()
{
#ifdef GR_CTRLPORT
    add_rpc_variable(rpcbasic_sptr(
        new rpcbasic_register_get<tag_debug, int>(alias(),
                                                  "num. tags",
                                                  &tag_debug::num_tags,
                                                  pmt::from_long(0),
                                                  pmt::from_long(10000),
                                                  pmt::from_long(0),
                                                  "",
                                                  "Number of Tags",
                                                  RPC_PRIVLVL_MIN,
                                                  DISPTIME | DISPOPTSTRIP)));
#endif /* GR_CTRLPORT */
}

} /* namespace blocks */
} /* namespace gr */
