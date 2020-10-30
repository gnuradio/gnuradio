/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_ALAW_ENCODER_SB_IMPL_H
#define INCLUDED_VOCODER_ALAW_ENCODER_SB_IMPL_H

#include <gnuradio/vocoder/alaw_encode_sb.h>

namespace gr {
namespace vocoder {

class alaw_encode_sb_impl : public alaw_encode_sb
{
public:
    alaw_encode_sb_impl();
    ~alaw_encode_sb_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_ALAW_ENCODE_SB_IMPL_H */
