/* -*- c++ -*- */
/*
 * Copyright 2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vector_insert_impl.h"
#include <gnuradio/block.h>
#include <gnuradio/io_signature.h>
#include <stdio.h>
#include <algorithm>
#include <stdexcept>

namespace gr {
namespace blocks {

template <class T>
typename vector_insert<T>::sptr
vector_insert<T>::make(const std::vector<T>& data, int periodicity, int offset)
{
    return gnuradio::get_initial_sptr(
        new vector_insert_impl<T>(data, periodicity, offset));
}

template <class T>
vector_insert_impl<T>::vector_insert_impl(const std::vector<T>& data,
                                          int periodicity,
                                          int offset)
    : block("vector_insert",
            io_signature::make(1, 1, sizeof(T)),
            io_signature::make(1, 1, sizeof(T))),
      d_data(data),
      d_offset(offset),
      d_periodicity(periodicity)
{
    this->set_tag_propagation_policy(gr::block::TPP_DONT); // handle tags manually
    // some sanity checks
    assert(offset < periodicity);
    assert(offset >= 0);
    assert((size_t)periodicity > data.size());
}

template <class T>
vector_insert_impl<T>::~vector_insert_impl()
{
}

template <class T>
int vector_insert_impl<T>::general_work(int noutput_items,
                                        gr_vector_int& ninput_items,
                                        gr_vector_const_void_star& input_items,
                                        gr_vector_void_star& output_items)
{
    T* out = (T*)output_items[0];
    const T* in = (const T*)input_items[0];

    int ii(0), oo(0);

    while ((oo < noutput_items) && (ii < ninput_items[0])) {
        // printf("oo = %d, ii = %d, d_offset = %d, noutput_items = %d, ninput_items[0] =
        // %d", oo, ii, d_offset, noutput_items, ninput_items[0]); printf(", d_periodicity
        // = %d\n", d_periodicity);

        if (d_offset >= ((int)d_data.size())) { // if we are in the copy region
            int max_copy = std::min(std::min(noutput_items - oo, ninput_items[0] - ii),
                                    d_periodicity - d_offset);
            std::vector<tag_t> tags;
            this->get_tags_in_range(
                tags, 0, this->nitems_read(0) + ii, this->nitems_read(0) + max_copy + ii);
            for (unsigned i = 0; i < tags.size(); i++) {
                // printf("copy tag from in@%d to out@%d\n", int(tags[i].offset),
                // int(nitems_written(0) + oo + (tags[i].offset-nitems_read(0)-ii)));
                this->add_item_tag(0,
                                   this->nitems_written(0) + oo +
                                       (tags[i].offset - this->nitems_read(0) - ii),
                                   tags[i].key,
                                   tags[i].value,
                                   tags[i].srcid);
            }
            // printf("copy %d from input\n", max_copy);
            memcpy(&out[oo], &in[ii], sizeof(T) * max_copy);
            // printf(" * memcpy returned.\n");
            ii += max_copy;
            oo += max_copy;
            d_offset = (d_offset + max_copy) % d_periodicity;
        } else { // if we are in the insertion region
            int max_copy = std::min(noutput_items - oo, ((int)d_data.size()) - d_offset);
            // printf("copy %d from d_data[%d] to out[%d]\n", max_copy, d_offset, oo);
            memcpy(&out[oo], &d_data[d_offset], sizeof(T) * max_copy);
            // printf(" * memcpy returned.\n");
            oo += max_copy;
            d_offset = (d_offset + max_copy) % d_periodicity;
            // printf(" ## (inelse) oo = %d, d_offset = %d\n", oo, d_offset);
        }

        // printf(" # exit else, on to next loop.\n");
    }
    // printf(" # got out of loop\n");

    // printf("consume = %d, produce = %d\n", ii, oo);
    this->consume_each(ii);
    return oo;
}

template class vector_insert<std::uint8_t>;
template class vector_insert<std::int16_t>;
template class vector_insert<std::int32_t>;
template class vector_insert<float>;
template class vector_insert<gr_complex>;
} /* namespace blocks */
} /* namespace gr */
