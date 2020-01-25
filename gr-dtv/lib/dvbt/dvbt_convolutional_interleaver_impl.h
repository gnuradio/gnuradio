/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_CONVOLUTIONAL_INTERLEAVER_IMPL_H
#define INCLUDED_DTV_DVBT_CONVOLUTIONAL_INTERLEAVER_IMPL_H

#include <gnuradio/dtv/dvbt_convolutional_interleaver.h>
#include <deque>
#include <vector>

namespace gr {
namespace dtv {

class dvbt_convolutional_interleaver_impl : public dvbt_convolutional_interleaver
{
private:
    int d_I;
    int d_M;
    std::vector<std::deque<unsigned char>*> d_shift;

public:
    dvbt_convolutional_interleaver_impl(int nsize, int I, int M);
    ~dvbt_convolutional_interleaver_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_CONVOLUTIONAL_INTERLEAVER_IMPL_H */
