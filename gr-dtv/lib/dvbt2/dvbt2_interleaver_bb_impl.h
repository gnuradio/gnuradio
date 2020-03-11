/* -*- c++ -*- */
/*
 * Copyright 2015,2018,2019 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT2_INTERLEAVER_BB_IMPL_H
#define INCLUDED_DTV_DVBT2_INTERLEAVER_BB_IMPL_H

#include "../dvb/dvb_defines.h"

#include <gnuradio/dtv/dvbt2_interleaver_bb.h>

namespace gr {
namespace dtv {

class dvbt2_interleaver_bb_impl : public dvbt2_interleaver_bb
{
private:
    int frame_size;
    int signal_constellation;
    int code_rate;
    int nbch;
    int q_val;
    int mod;
    int packed_items;
    unsigned char tempu[FRAME_SIZE_NORMAL];
    int lookup_table[FRAME_SIZE_NORMAL];

    void generate_lookup();
    inline void interleave_parity_bits(int* tempu, const int*& in);
    inline void
    twist_interleave_columns(int* tempv, int* tempu, int rows, int mod, const int* twist);

    const static int twist16n[8];
    const static int twist64n[12];
    const static int twist256n[16];

    const static int twist16s[8];
    const static int twist64s[12];
    const static int twist256s[8];

    const static int mux16[8];
    const static int mux64[12];
    const static int mux256[16];

    const static int mux16_35[8];
    const static int mux16_13[8];
    const static int mux16_25[8];
    const static int mux64_35[12];
    const static int mux64_13[12];
    const static int mux64_25[12];
    const static int mux256_35[16];
    const static int mux256_23[16];

    const static int mux256s[8];
    const static int mux256s_13[8];
    const static int mux256s_25[8];

public:
    dvbt2_interleaver_bb_impl(dvb_framesize_t framesize,
                              dvb_code_rate_t rate,
                              dvb_constellation_t constellation);
    ~dvbt2_interleaver_bb_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_INTERLEAVER_BB_IMPL_H */
