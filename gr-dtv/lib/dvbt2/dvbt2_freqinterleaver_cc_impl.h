/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT2_FREQINTERLEAVER_CC_IMPL_H
#define INCLUDED_DTV_DVBT2_FREQINTERLEAVER_CC_IMPL_H

#include "../dvb/dvb_defines.h"

#include <gnuradio/dtv/dvbt2_freqinterleaver_cc.h>

namespace gr {
namespace dtv {

class dvbt2_freqinterleaver_cc_impl : public dvbt2_freqinterleaver_cc
{
private:
    int interleaved_items;
    int num_data_symbols;
    int Heven[32768];
    int Hodd[32768];
    int HevenP2[32768];
    int HoddP2[32768];
    int HevenFC[32768];
    int HoddFC[32768];
    int N_P2;
    int C_P2;
    int N_FC;
    int C_FC;
    int C_DATA;

    const static int bitperm1keven[9];
    const static int bitperm1kodd[9];
    const static int bitperm2keven[10];
    const static int bitperm2kodd[10];
    const static int bitperm4keven[11];
    const static int bitperm4kodd[11];
    const static int bitperm8keven[12];
    const static int bitperm8kodd[12];
    const static int bitperm16keven[13];
    const static int bitperm16kodd[13];
    const static int bitperm32k[14];

public:
    dvbt2_freqinterleaver_cc_impl(dvbt2_extended_carrier_t carriermode,
                                  dvbt2_fftsize_t fftsize,
                                  dvbt2_pilotpattern_t pilotpattern,
                                  dvb_guardinterval_t guardinterval,
                                  int numdatasyms,
                                  dvbt2_papr_t paprmode,
                                  dvbt2_version_t version,
                                  dvbt2_preamble_t preamble);
    ~dvbt2_freqinterleaver_cc_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_FREQINTERLEAVER_CC_IMPL_H */
