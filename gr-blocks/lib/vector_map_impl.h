/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_VECTOR_MAP_IMPL_H
#define INCLUDED_GR_VECTOR_MAP_IMPL_H

#include <gnuradio/blocks/vector_map.h>
#include <gnuradio/thread/thread.h>

namespace gr {
namespace blocks {

class vector_map_impl : public vector_map
{
private:
    const size_t d_item_size;
    const std::vector<size_t> d_in_vlens;
    std::vector<std::vector<std::vector<size_t>>> d_mapping;
    gr::thread::mutex d_mutex; // mutex to protect set/work access

public:
    vector_map_impl(size_t item_size,
                    std::vector<size_t> in_vlens,
                    std::vector<std::vector<std::vector<size_t>>> mapping);
    ~vector_map_impl() override;

    void set_mapping(std::vector<std::vector<std::vector<size_t>>> mapping) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_VECTOR_MAP_IMPL_H */
