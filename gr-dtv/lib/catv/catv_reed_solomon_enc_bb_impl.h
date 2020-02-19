/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_IMPL_H
#define INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_IMPL_H

#include <gnuradio/dtv/catv_reed_solomon_enc_bb.h>

namespace gr {
namespace dtv {

class catv_reed_solomon_enc_bb_impl : public catv_reed_solomon_enc_bb
{
private:
    unsigned char gf_mul_table[128][128];
    unsigned char gf_exp[256];
    unsigned char gf_log[128];

    void init_rs();
    unsigned char gf_poly_eval(unsigned char* p, int len, unsigned char x);
    void reed_solomon_enc(const unsigned char* message, unsigned char* output);

public:
    catv_reed_solomon_enc_bb_impl();
    ~catv_reed_solomon_enc_bb_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_REED_SOLOMON_ENC_BB_IMPL_H */
