/* -*- c++ -*- */
/*
 * Copyright 2015,2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBS2_PHYSICAL_CC_IMPL_H
#define INCLUDED_DTV_DVBS2_PHYSICAL_CC_IMPL_H

#include "../dvb/dvb_defines.h"

#include <gnuradio/dtv/dvbs2_physical_cc.h>

#define VLSNR_HEADER_LENGTH 900

namespace gr {
namespace dtv {

class dvbs2_physical_cc_impl : public dvbs2_physical_cc
{
private:
    int frame_size;
    int signal_constellation;
    int slots;
    int pilot_mode;
    int pilot_symbols;
    int gold_code;
    int vlsnr_header;
    int vlsnr_set;
    int b[VLSNR_HEADER_LENGTH];
    gr_complex m_bpsk[4][2];
    gr_complex m_pl[90];
    gr_complex m_vlsnr_header[VLSNR_HEADER_LENGTH];
    gr_complex m_zero;
    int m_cscram[FRAME_SIZE_NORMAL];
    void b_64_8_code(unsigned char, int*);
    void pl_header_encode(unsigned char, unsigned char, int*);
    int parity_chk(long, long);
    void build_symbol_scrambler_table(void);

    const static unsigned long g[7];
    const static int ph_scram_tab[64];
    const static int ph_sync_seq[26];
    const static int ph_vlsnr_seq[16][VLSNR_HEADER_LENGTH - 4];

public:
    dvbs2_physical_cc_impl(dvb_framesize_t framesize,
                           dvb_code_rate_t rate,
                           dvb_constellation_t constellation,
                           dvbs2_pilots_t pilots,
                           int goldcode);
    ~dvbs2_physical_cc_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBS2_PHYSICAL_CC_IMPL_H */
