/* -*- c++ -*- */
/*
 * Copyright 2011.2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_ALAW_DECODE_BS_IMPL_H
#define INCLUDED_VOCODER_ALAW_DECODE_BS_IMPL_H

#include <gnuradio/vocoder/alaw_decode_bs.h>

namespace gr {
namespace vocoder {

class alaw_decode_bs_impl : public alaw_decode_bs
{
public:
    alaw_decode_bs_impl();
    ~alaw_decode_bs_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_ALAW_DECODE_BS_IMPL_H */
