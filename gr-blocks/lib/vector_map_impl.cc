/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "vector_map_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>

namespace gr {
namespace blocks {

std::vector<int> get_in_sizeofs(size_t item_size, std::vector<size_t> in_vlens)
{
    std::vector<int> in_sizeofs;
    for (unsigned int i = 0; i < in_vlens.size(); i++) {
        in_sizeofs.push_back(in_vlens[i] * item_size);
    }
    return in_sizeofs;
}

std::vector<int> get_out_sizeofs(size_t item_size,
                                 std::vector<std::vector<std::vector<size_t>>> mapping)
{
    std::vector<int> out_sizeofs;
    for (unsigned int i = 0; i < mapping.size(); i++) {
        out_sizeofs.push_back(mapping[i].size() * item_size);
    }
    return out_sizeofs;
}

vector_map::sptr vector_map::make(size_t item_size,
                                  std::vector<size_t> in_vlens,
                                  std::vector<std::vector<std::vector<size_t>>> mapping)
{
    return gnuradio::make_block_sptr<vector_map_impl>(item_size, in_vlens, mapping);
}

vector_map_impl::vector_map_impl(size_t item_size,
                                 std::vector<size_t> in_vlens,
                                 std::vector<std::vector<std::vector<size_t>>> mapping)
    : sync_block(
          "vector_map",
          io_signature::makev(
              in_vlens.size(), in_vlens.size(), get_in_sizeofs(item_size, in_vlens)),
          io_signature::makev(
              mapping.size(), mapping.size(), get_out_sizeofs(item_size, mapping))),
      d_item_size(item_size),
      d_in_vlens(in_vlens)
{
    set_mapping(mapping);
}

vector_map_impl::~vector_map_impl() {}

void vector_map_impl::set_mapping(std::vector<std::vector<std::vector<size_t>>> mapping)
{
    // Make sure the contents of the mapping vectors are possible.
    for (unsigned int i = 0; i < mapping.size(); i++) {
        for (unsigned int j = 0; j < mapping[i].size(); j++) {
            if (mapping[i][j].size() != 2) {
                throw std::runtime_error(
                    "Mapping must be of the form (out_mapping_stream1, "
                    "out_mapping_stream2, ...), where out_mapping_stream1 is of the form "
                    "(mapping_element1, mapping_element2, ...), where mapping_element1 "
                    "is of the form (input_stream, input_element).  This error is raised "
                    "because a mapping_element vector does not contain exactly 2 items.");
            }
            unsigned int s = mapping[i][j][0];
            unsigned int index = mapping[i][j][1];
            if (s >= d_in_vlens.size()) {
                throw std::runtime_error("Stream numbers in mapping must be less than "
                                         "the number of input streams.");
            }
            if (index >= d_in_vlens[s]) {
                throw std::runtime_error("Indices in mapping must be greater than 0 and "
                                         "less than the input vector lengths.");
            }
        }
    }
    gr::thread::scoped_lock guard(d_mutex);
    d_mapping = mapping;
}

int vector_map_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    const char** inv = (const char**)&input_items[0];
    char** outv = (char**)&output_items[0];

    for (unsigned int n = 0; n < (unsigned int)(noutput_items); n++) {
        for (unsigned int i = 0; i < d_mapping.size(); i++) {
            unsigned int out_vlen = d_mapping[i].size();
            for (unsigned int j = 0; j < out_vlen; j++) {
                unsigned int s = d_mapping[i][j][0];
                unsigned int k = d_mapping[i][j][1];
                memcpy(outv[i] + out_vlen * d_item_size * n + d_item_size * j,
                       inv[s] + d_in_vlens[s] * d_item_size * n + k * d_item_size,
                       d_item_size);
            }
        }
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
