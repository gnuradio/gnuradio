/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SCRAMBLER_BB_IMPL_H
#define INCLUDED_GR_SCRAMBLER_BB_IMPL_H

#include <gnuradio/digital/lfsr.h>
#include <gnuradio/digital/scrambler_bb.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace digital {

class scrambler_bb_impl : public scrambler_bb
{
private:
    digital::lfsr d_lfsr;

public:
    scrambler_bb_impl(int mask, int seed, int len);
    ~scrambler_bb_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_SCRAMBLER_BB_IMPL_H */
