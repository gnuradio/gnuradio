/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT2_P1INSERTION_CC_IMPL_H
#define INCLUDED_DTV_DVBT2_P1INSERTION_CC_IMPL_H

#include "../dvb/dvb_defines.h"

#include <gnuradio/dtv/dvbt2_p1insertion_cc.h>
#include <gnuradio/fft/fft.h>

namespace gr {
namespace dtv {

class dvbt2_p1insertion_cc_impl : public dvbt2_p1insertion_cc
{
private:
    int frame_items;
    int insertion_items;
    int p1_randomize[384];
    int modulation_sequence[384];
    int dbpsk_modulation_sequence[385];
    gr_complex p1_freq[1024];
    gr_complex p1_freqshft[1024];
    gr_complex p1_time[1024];
    gr_complex p1_timeshft[1024];
    void init_p1_randomizer(void);

    int show_levels;
    float real_positive;
    float real_negative;
    float imag_positive;
    float imag_negative;
    float real_positive_threshold;
    float real_negative_threshold;
    float imag_positive_threshold;
    float imag_negative_threshold;
    int real_positive_threshold_count;
    int real_negative_threshold_count;
    int imag_positive_threshold_count;
    int imag_negative_threshold_count;

    fft::fft_complex_rev p1_fft;

    const static int p1_active_carriers[384];
    const static unsigned char s1_modulation_patterns[8][8];
    const static unsigned char s2_modulation_patterns[16][32];

public:
    dvbt2_p1insertion_cc_impl(dvbt2_extended_carrier_t carriermode,
                              dvbt2_fftsize_t fftsize,
                              dvb_guardinterval_t guardinterval,
                              int numdatasyms,
                              dvbt2_preamble_t preamble,
                              dvbt2_showlevels_t showlevels,
                              float vclip);
    ~dvbt2_p1insertion_cc_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_P1INSERTION_CC_IMPL_H */
