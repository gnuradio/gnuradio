/* -*- c++ -*- */
/*
 * Copyright 2006.2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef UNPACKED_TO_PACKED_IMPL_H
#define UNPACKED_TO_PACKED_IMPL_H

#include <gnuradio/blocks/unpacked_to_packed.h>

namespace gr {
namespace blocks {

template <class T>
class unpacked_to_packed_impl : public unpacked_to_packed<T>
{
private:
    const unsigned int d_bits_per_chunk;
    const endianness_t d_endianness;
    unsigned int d_index;
    static constexpr unsigned int d_bits_per_type = sizeof(T) * 8;
    unsigned int
    get_bit_be1(const T* in_vector, unsigned int bit_addr, unsigned int bits_per_chunk);

public:
    unpacked_to_packed_impl(unsigned int bits_per_chunk, endianness_t endianness);
    ~unpacked_to_packed_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;
    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    bool check_topology(int ninputs, int noutputs) override
    {
        return ninputs == noutputs;
    }
};

} /* namespace blocks */
} /* namespace gr */

#endif /* UNPACKED_TO_PACKED_IMPL_H */
