/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 * Copyright 2021 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_DIFF_ENCODER_BB_IMPL_H
#define INCLUDED_GR_DIFF_ENCODER_BB_IMPL_H

#include <gnuradio/digital/diff_encoder_bb.h>

namespace gr {
namespace digital {

class diff_encoder_bb_impl : public diff_encoder_bb
{
public:
    diff_encoder_bb_impl(unsigned int modulus,
                         enum diff_coding_type coding = DIFF_DIFFERENTIAL);
    ~diff_encoder_bb_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

private:
    unsigned int d_last_out;
    const unsigned int d_modulus;
    const enum diff_coding_type d_coding;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_DIFF_ENCODER_BB_IMPL_H */
