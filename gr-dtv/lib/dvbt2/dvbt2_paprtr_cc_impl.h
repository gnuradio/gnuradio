/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT2_PAPRTR_CC_IMPL_H
#define INCLUDED_DTV_DVBT2_PAPRTR_CC_IMPL_H

#include "../dvb/dvb_defines.h"

#include <gnuradio/dtv/dvbt2_paprtr_cc.h>
#include <gnuradio/fft/fft.h>

#define MAX_CARRIERS 27841
#define MAX_FFTSIZE 32768
#define MAX_PAPRTONES 288

enum dvbt2_carrier_type_t {
    DATA_CARRIER = 1,
    P2PILOT_CARRIER,
    P2PAPR_CARRIER,
    TRPAPR_CARRIER,
    SCATTERED_CARRIER,
    CONTINUAL_CARRIER
};

namespace gr {
namespace dtv {

class dvbt2_paprtr_cc_impl : public dvbt2_paprtr_cc
{
private:
    fft::fft_complex_rev papr_fft;
    int papr_fft_size;
    int num_symbols;
    int fft_size;
    int left_nulls;
    int right_nulls;
    int carrier_mode;
    int papr_mode;
    int version_num;
    double v_clip;
    int num_iterations;
    const int* p2_papr_map;
    const int* tr_papr_map;
    int p2_carrier_map[MAX_CARRIERS];
    int data_carrier_map[MAX_CARRIERS];
    int fc_carrier_map[MAX_CARRIERS];
    volk::vector<gr_complex> ones_freq;
    volk::vector<gr_complex> ones_time;
    volk::vector<gr_complex> c;
    volk::vector<gr_complex> ctemp;
    volk::vector<float> magnitude;
    volk::vector<gr_complex> r;
    volk::vector<gr_complex> rNew;
    volk::vector<gr_complex> v;
    float alphaLimit[MAX_PAPRTONES];
    float alphaLimitMax[MAX_PAPRTONES];
    int N_P2;
    int N_FC;
    int K_EXT;
    int C_PS;
    int N_TR;
    int dx;
    int dy;
    int shift;
    void init_pilots(int);

    const static int p2_papr_map_1k[10];
    const static int p2_papr_map_2k[18];
    const static int p2_papr_map_4k[36];
    const static int p2_papr_map_8k[72];
    const static int p2_papr_map_16k[144];
    const static int p2_papr_map_32k[288];
    const static int tr_papr_map_1k[10];
    const static int tr_papr_map_2k[18];
    const static int tr_papr_map_4k[36];
    const static int tr_papr_map_8k[72];
    const static int tr_papr_map_16k[144];
    const static int tr_papr_map_32k[288];

public:
    dvbt2_paprtr_cc_impl(dvbt2_extended_carrier_t carriermode,
                         dvbt2_fftsize_t fftsize,
                         dvbt2_pilotpattern_t pilotpattern,
                         dvb_guardinterval_t guardinterval,
                         int numdatasyms,
                         dvbt2_papr_t paprmode,
                         dvbt2_version_t version,
                         float vclip,
                         int iterations,
                         unsigned int vlength);

    ~dvbt2_paprtr_cc_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_PAPRTR_CC_IMPL_H */
