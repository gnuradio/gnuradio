/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_REINFLATE_BB_IMPL_H
#define INCLUDED_FEC_REINFLATE_BB_IMPL_H

#include <gnuradio/fec/depuncture_bb.h>

namespace gr {
namespace fec {

class FEC_API depuncture_bb_impl : public depuncture_bb
{
private:
    int d_puncsize;
    int d_delay;
    int d_puncholes;
    int d_puncpat;
    char d_sym;

public:
    depuncture_bb_impl(int puncsize, int puncpat, int delay = 0, char symbol = 127);
    ~depuncture_bb_impl();

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
    int fixed_rate_ninput_to_noutput(int ninput);
    int fixed_rate_noutput_to_ninput(int noutput);
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);
};

} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_DEPUNCTURE_BB_IMPL_H */
