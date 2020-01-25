/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_DIFF_DECODER_BB_IMPL_H
#define INCLUDED_GR_DIFF_DECODER_BB_IMPL_H

#include <gnuradio/digital/diff_decoder_bb.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

class diff_decoder_bb_impl : public diff_decoder_bb
{
public:
    diff_decoder_bb_impl(unsigned int modulus);
    ~diff_decoder_bb_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

private:
    unsigned int d_modulus;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_DIFF_DECODER_BB_IMPL_H */
