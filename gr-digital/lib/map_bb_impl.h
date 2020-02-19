/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_MAP_BB_IMPL_H
#define INCLUDED_GR_MAP_BB_IMPL_H

#include <gnuradio/digital/map_bb.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace digital {

class map_bb_impl : public map_bb
{
private:
    static constexpr size_t s_map_size = 0x100;
    unsigned char d_map[s_map_size];
    mutable gr::thread::mutex d_mutex;

public:
    map_bb_impl(const std::vector<int>& map);
    ~map_bb_impl();

    void set_map(const std::vector<int>& map);
    std::vector<int> map() const;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_MAP_BB_IMPL_H */
