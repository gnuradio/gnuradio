/* -*- c++ -*- */
/*
 * Copyright 2004,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "packed_to_unpacked_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace blocks {

template <class T>
typename packed_to_unpacked<T>::sptr
packed_to_unpacked<T>::make(unsigned int bits_per_chunk, endianness_t endianness)
{
    return gnuradio::make_block_sptr<packed_to_unpacked_impl<T>>(bits_per_chunk,
                                                                 endianness);
}

template <class T>
packed_to_unpacked_impl<T>::packed_to_unpacked_impl(unsigned int bits_per_chunk,
                                                    endianness_t endianness)
    : block("packed_to_unpacked",
            io_signature::make(1, -1, sizeof(T)),
            io_signature::make(1, -1, sizeof(T))),
      d_bits_per_chunk(bits_per_chunk),
      d_endianness(endianness),
      d_index(0)
{
    if (bits_per_chunk > d_bits_per_type) {
        this->d_logger->error("Requested to get {:d} out of a {:d} bit chunk",
                              bits_per_chunk,
                              d_bits_per_type);
        throw std::domain_error("can't have more bits in chunk than in output type");
    }

    this->set_relative_rate((uint64_t)this->d_bits_per_type, (uint64_t)bits_per_chunk);
}

template <class T>
packed_to_unpacked_impl<T>::~packed_to_unpacked_impl()
{
}

template <class T>
void packed_to_unpacked_impl<T>::forecast(int noutput_items,
                                          gr_vector_int& ninput_items_required)
{
    const int input_required = (int)ceil((d_index + noutput_items * d_bits_per_chunk) /
                                         (1.0 * this->d_bits_per_type));
    const unsigned ninputs = ninput_items_required.size();
    for (unsigned int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = input_required;
        // printf("Forecast wants %d needs %d\n",noutput_items,ninput_items_required[i]);
    }
}

template <class T>
unsigned int packed_to_unpacked_impl<T>::log2_l_type()
{
    unsigned int val = sizeof(T);
    if (!val || (val & (val - 1))) {
        return 0;
    }
    unsigned int ld = 0;
    while (val >>= 1) {
        ld++;
    }
    return ld + 3;
};

template <class T>
unsigned int packed_to_unpacked_impl<T>::get_bit_le(const T* in_vector,
                                                    unsigned int bit_addr)
{
    const T x = in_vector[bit_addr >> this->log2_l_type()];
    return (x >> (bit_addr & (this->d_bits_per_type - 1))) & 1;
}

template <class T>
unsigned int packed_to_unpacked_impl<T>::get_bit_be(const T* in_vector,
                                                    unsigned int bit_addr)
{
    const T x = in_vector[bit_addr >> this->log2_l_type()];
    return (x >>
            ((this->d_bits_per_type - 1) - (bit_addr & (this->d_bits_per_type - 1)))) &
           1;
}

template <class T>
int packed_to_unpacked_impl<T>::general_work(int noutput_items,
                                             gr_vector_int& ninput_items,
                                             gr_vector_const_void_star& input_items,
                                             gr_vector_void_star& output_items)
{
    unsigned int index_tmp = d_index;

    const int nstreams = input_items.size();

    for (int m = 0; m < nstreams; m++) {
        const T* in = (const T*)input_items[m];
        T* out = (T*)output_items[m];
        index_tmp = d_index;

        // per stream processing

        switch (d_endianness) {
        case GR_MSB_FIRST:
            for (int i = 0; i < noutput_items; i++) {
                // printf("here msb %d\n",i);
                T x = 0;
                for (unsigned int j = 0; j < d_bits_per_chunk; j++, index_tmp++)
                    x = (x << 1) | get_bit_be(in, index_tmp);
                out[i] = x;
            }
            break;

        case GR_LSB_FIRST:
            for (int i = 0; i < noutput_items; i++) {
                // printf("here lsb %d\n",i);
                T x = 0;
                for (unsigned int j = 0; j < d_bits_per_chunk; j++, index_tmp++)
                    x = (x << 1) | get_bit_le(in, index_tmp);
                out[i] = x;
            }
            break;

        default:
            this->d_logger->error("unknown endianness");
            throw std::runtime_error("unknown endianness");
        }
    }

    d_index = index_tmp;
    this->consume_each(d_index >> this->log2_l_type());
    d_index = d_index & (this->d_bits_per_type - 1);
    // printf("got to end\n");
    return noutput_items;
}

template class packed_to_unpacked<std::uint8_t>;
template class packed_to_unpacked<std::int16_t>;
template class packed_to_unpacked<std::int32_t>;
} /* namespace blocks */
} /* namespace gr */
