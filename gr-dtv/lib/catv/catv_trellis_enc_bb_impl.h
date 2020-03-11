/* -*- c++ -*- */
/*
 * Copyright 2016,2017 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_TRELLIS_ENC_BB_IMPL_H
#define INCLUDED_DTV_CATV_TRELLIS_ENC_BB_IMPL_H

#include <gnuradio/dtv/catv_trellis_enc_bb.h>

namespace gr {
namespace dtv {

class catv_trellis_enc_bb_impl : public catv_trellis_enc_bb
{
private:
    unsigned char diff_precoder_table[4][16][16][3];
    unsigned char G1table[32];
    unsigned char G2table[32];
    unsigned char trellis_table_x[16][16][6];
    unsigned char trellis_table_y[16][16][6];
    unsigned char Xq, Yq, XYp;
    int signal_constellation;
    int trellis_group;

    void
    diff_precoder(unsigned char W, unsigned char Z, unsigned char* Xp, unsigned char* Yp);
    void init_trellis();
    void trellis_code_64qam(const unsigned char* rs, unsigned char* qs);
    void trellis_code_256qam(const unsigned char* rs, unsigned char* qs);

public:
    catv_trellis_enc_bb_impl(catv_constellation_t constellation);
    ~catv_trellis_enc_bb_impl();

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_TRELLIS_ENC_BB_IMPL_H */
