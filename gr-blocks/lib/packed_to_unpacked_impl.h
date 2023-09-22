/* -*- c++ -*- */
/*
 * Copyright 2006,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef PACKED_TO_UNPACKED_IMPL_H
#define PACKED_TO_UNPACKED_IMPL_H

#include <gnuradio/blocks/packed_to_unpacked.h>

namespace gr {
namespace blocks {

template <class T>
class packed_to_unpacked_impl : public packed_to_unpacked<T>
{
private:
    const unsigned int d_bits_per_chunk;
    const endianness_t d_endianness;
    unsigned int d_index;
    static constexpr unsigned int d_bits_per_type = sizeof(T) * 8;
    static unsigned int log2_l_type();
    unsigned int get_bit_le(const T* in_vector, unsigned int bit_addr);
    unsigned int get_bit_be(const T* in_vector, unsigned int bit_addr);

public:
    packed_to_unpacked_impl(unsigned int bits_per_chunk, endianness_t endianness);
    ~packed_to_unpacked_impl() override;

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

#endif /* PACKED_TO_UNPACKED_IMPL_H */
