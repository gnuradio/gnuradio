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
    scrambler_bb_impl(uint64_t mask, uint64_t seed, uint8_t len);
    ~scrambler_bb_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_SCRAMBLER_BB_IMPL_H */
