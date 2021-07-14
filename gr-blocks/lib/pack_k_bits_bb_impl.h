/* -*- c++ -*- */
/*
 * Copyright 2012-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_PACK_K_BITS_BB_IMPL_H
#define INCLUDED_GR_PACK_K_BITS_BB_IMPL_H

#include <gnuradio/blocks/pack_k_bits.h>
#include <gnuradio/blocks/pack_k_bits_bb.h>

namespace gr {
namespace blocks {

class pack_k_bits_bb_impl : public pack_k_bits_bb
{
private:
    const kernel::pack_k_bits d_pack;

    unsigned d_k;

public:
    pack_k_bits_bb_impl(unsigned k);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_PACK_K_BITS_BB_IMPL_H */
