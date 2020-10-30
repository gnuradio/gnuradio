/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_G723_40_DECODE_BS_IMPL_H
#define INCLUDED_VOCODER_G723_40_DECODE_BS_IMPL_H

#include <gnuradio/vocoder/g723_40_decode_bs.h>

extern "C" {
#include "g7xx/g72x.h"
}

namespace gr {
namespace vocoder {

class g723_40_decode_bs_impl : public g723_40_decode_bs
{
private:
    g72x_state d_state;

public:
    g723_40_decode_bs_impl();
    ~g723_40_decode_bs_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_G723_40_DECODE_BS_IMPL_H */
