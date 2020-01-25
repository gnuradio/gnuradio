/* -*- c++ -*- */
/*
 * Copyright 2015,2016,2019 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVB_BCH_BB_IMPL_H
#define INCLUDED_DTV_DVB_BCH_BB_IMPL_H

#include "dvb_defines.h"
#include <gnuradio/dtv/dvb_bch_bb.h>
#include <bitset>

#define MAX_BCH_PARITY_BITS 192

namespace gr {
namespace dtv {

class dvb_bch_bb_impl : public dvb_bch_bb
{
private:
    unsigned int kbch;
    unsigned int nbch;
    unsigned int bch_code;
    unsigned int frame_size;

    std::bitset<MAX_BCH_PARITY_BITS> crc_table[256];
    std::bitset<MAX_BCH_PARITY_BITS> crc_medium_table[16];
    unsigned int num_parity_bits;
    std::bitset<MAX_BCH_PARITY_BITS> polynome;

    void calculate_crc_table();
    void calculate_medium_crc_table();
    int poly_mult(const int*, int, const int*, int, int*);
    void bch_poly_build_tables(void);

public:
    dvb_bch_bb_impl(dvb_standard_t standard,
                    dvb_framesize_t framesize,
                    dvb_code_rate_t rate);
    ~dvb_bch_bb_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_BCH_BB_IMPL_H */
