/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_SYMBOL_INNER_INTERLEAVER_IMPL_H
#define INCLUDED_DTV_DVBT_SYMBOL_INNER_INTERLEAVER_IMPL_H

#include "dvbt_configure.h"
#include <gnuradio/dtv/dvbt_symbol_inner_interleaver.h>

namespace gr {
namespace dtv {

class dvbt_symbol_inner_interleaver_impl : public dvbt_symbol_inner_interleaver
{
private:
    const dvbt_configure config;

    int d_symbols_per_frame;
    dvbt_transmission_mode_t d_transmission_mode;
    int d_nsize;
    int d_direction;
    int d_fft_length;
    int d_payload_length;

    int* d_h;
    const char* d_bit_perm;
    static const char d_bit_perm_2k[];
    static const char d_bit_perm_8k[];

    // Keeps the symbol index
    unsigned int d_symbol_index;

    void generate_H();
    int H(int q);
    int calculate_R(int i);

public:
    dvbt_symbol_inner_interleaver_impl(int nsize,
                                       dvbt_transmission_mode_t transmission,
                                       int direction);
    ~dvbt_symbol_inner_interleaver_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_SYMBOL_INNER_INTERLEAVER_IMPL_H */
