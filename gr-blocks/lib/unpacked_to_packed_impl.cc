/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "unpacked_to_packed_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace blocks {


template <class T>
typename unpacked_to_packed<T>::sptr
unpacked_to_packed<T>::make(unsigned int bits_per_chunk, endianness_t endianness)
{
    return gnuradio::make_block_sptr<unpacked_to_packed_impl<T>>(bits_per_chunk,
                                                                 endianness);
}

template <class T>
unpacked_to_packed_impl<T>::unpacked_to_packed_impl(unsigned int bits_per_chunk,
                                                    endianness_t endianness)
    : block("unpacked_to_packed",
            io_signature::make(1, -1, sizeof(T)),
            io_signature::make(1, -1, sizeof(T))),
      d_bits_per_chunk(bits_per_chunk),
      d_endianness(endianness),
      d_index(0)
{
    if (bits_per_chunk > d_bits_per_type) {
        this->d_logger->error(
            "Requested to put {:d} in a {:d} bit chunk", bits_per_chunk, d_bits_per_type);
        throw std::domain_error("can't have more bits in chunk than in output type");
    }

    this->set_relative_rate((uint64_t)bits_per_chunk, (uint64_t)this->d_bits_per_type);
}

template <class T>
unpacked_to_packed_impl<T>::~unpacked_to_packed_impl()
{
}

template <class T>
void unpacked_to_packed_impl<T>::forecast(int noutput_items,
                                          gr_vector_int& ninput_items_required)
{
    const int input_required =
        (int)ceil((d_index + noutput_items * 1.0 * d_bits_per_type) / d_bits_per_chunk);
    const unsigned ninputs = ninput_items_required.size();
    for (unsigned int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = input_required;
    }
}

template <class T>
unsigned int unpacked_to_packed_impl<T>::get_bit_be1(const T* in_vector,
                                                     unsigned int bit_addr,
                                                     unsigned int bits_per_chunk)
{
    const unsigned int byte_addr = bit_addr / bits_per_chunk;
    const T x = in_vector[byte_addr];
    const unsigned int residue = bit_addr - byte_addr * bits_per_chunk;
    // printf("Bit addr %d  byte addr %d  residue %d  val
    // %d\n",bit_addr,byte_addr,residue,(x>>(bits_per_chunk-1-residue))&1);
    return (x >> (bits_per_chunk - 1 - residue)) & 1;
}

template <class T>
int unpacked_to_packed_impl<T>::general_work(int noutput_items,
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

        // assert((ninput_items[m]-d_index)*d_bits_per_chunk >=
        // noutput_items*d_bits_per_type);

        switch (d_endianness) {

        case GR_MSB_FIRST:
            for (int i = 0; i < noutput_items; i++) {
                T tmp = 0;
                for (unsigned int j = 0; j < d_bits_per_type; j++) {
                    tmp = (tmp << 1) | get_bit_be1(in, index_tmp, d_bits_per_chunk);
                    index_tmp++;
                }
                out[i] = tmp;
            }
            break;

        case GR_LSB_FIRST:
            for (int i = 0; i < noutput_items; i++) {
                unsigned long tmp = 0;
                for (unsigned int j = 0; j < d_bits_per_type; j++) {
                    tmp = (tmp >> 1) | (get_bit_be1(in, index_tmp, d_bits_per_chunk)
                                        << (d_bits_per_type - 1));
                    index_tmp++;
                }
                out[i] = tmp;
            }
            break;

        default:
            this->d_logger->error("unknown endianness");
            throw std::runtime_error("unknown endianness");
        }
    }

    d_index = index_tmp;
    this->consume_each((int)(d_index / d_bits_per_chunk));
    d_index = d_index % d_bits_per_chunk;

    return noutput_items;
}

template class unpacked_to_packed<std::uint8_t>;
template class unpacked_to_packed<std::int16_t>;
template class unpacked_to_packed<std::int32_t>;
} /* namespace blocks */
} /* namespace gr */
