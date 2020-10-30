/* -*- c++ -*- */
/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_ULAW_ENCODER_SB_IMPL_H
#define INCLUDED_VOCODER_ULAW_ENCODER_SB_IMPL_H

#include <gnuradio/vocoder/ulaw_encode_sb.h>

namespace gr {
namespace vocoder {

class ulaw_encode_sb_impl : public ulaw_encode_sb
{
public:
    ulaw_encode_sb_impl();
    ~ulaw_encode_sb_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_ULAW_ENCODE_SB_IMPL_H */
