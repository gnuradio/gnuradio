/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_CONVOLUTIONAL_DEINTERLEAVER_IMPL_H
#define INCLUDED_DTV_DVBT_CONVOLUTIONAL_DEINTERLEAVER_IMPL_H

#include <gnuradio/dtv/dvbt_convolutional_deinterleaver.h>

namespace gr {
namespace dtv {

class dvbt_convolutional_deinterleaver_impl : public dvbt_convolutional_deinterleaver
{
private:
    static const int d_SYNC;
    static const int d_NSYNC;
    static const int d_MUX_PKT;

    int d_blocks;
    int d_I;
    int d_M;
    std::vector<std::deque<unsigned char>*> d_shift;

public:
    dvbt_convolutional_deinterleaver_impl(int nsize, int I, int M);
    ~dvbt_convolutional_deinterleaver_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_CONVOLUTIONAL_DEINTERLEAVER_IMPL_H */
