/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_PUNCTURE_FF_IMPL_H
#define INCLUDED_FEC_PUNCTURE_FF_IMPL_H

#include <gnuradio/fec/puncture_ff.h>

namespace gr {
namespace fec {

class FEC_API puncture_ff_impl : public puncture_ff
{
private:
    int d_puncsize;
    int d_delay;
    int d_puncholes;
    int d_puncpat;

public:
    puncture_ff_impl(int puncsize, int puncpat, int delay);
    ~puncture_ff_impl();

    // void catch_msg(pmt::pmt_t msg);

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

#endif /* INCLUDED_FEC_PUNCTURE_FF_IMPL_H */
