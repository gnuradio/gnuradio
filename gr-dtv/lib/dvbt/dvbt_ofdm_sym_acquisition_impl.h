/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_DVBT_OFDM_SYM_ACQUISITION_IMPL_H
#define INCLUDED_DTV_DVBT_OFDM_SYM_ACQUISITION_IMPL_H

#include <gnuradio/dtv/dvbt_ofdm_sym_acquisition.h>
#include <volk/volk_alloc.hh>

namespace gr {
namespace dtv {

class dvbt_ofdm_sym_acquisition_impl : public dvbt_ofdm_sym_acquisition
{
private:
    const int d_fft_length;
    const int d_cp_length;
    const float d_rho;

    volk::vector<gr_complex> d_conj;
    volk::vector<float> d_norm;
    volk::vector<gr_complex> d_corr;
    volk::vector<gr_complex> d_gamma;
    volk::vector<float> d_lambda;
    std::vector<int> d_ml_sync_peak_pos;
    std::vector<float> d_ml_sync_phi;
    // For peak detector
    float d_threshold_factor_rise;
    float d_avg_alpha;
    float d_avg_min;
    float d_avg_max;
    float d_phase = 0.0;
    double d_phaseinc = 0.0;
    int d_cp_found = 0;
    double d_nextphaseinc = 0;
    int d_nextpos = 0;

    int d_initial_acquisition = 0;

    int d_cp_start = 0;

    volk::vector<gr_complex> d_derot;
    int d_to_consume = 0;
    int d_to_out = 0;
    int d_consumed = 0;
    int d_out = 0;

    int ml_sync(const gr_complex* in,
                int lookup_start,
                int lookup_stop,
                int* cp_pos,
                gr_complex* derot,
                int* to_consume,
                int* to_out);

    int peak_detect_init(float threshold_factor_rise, float alpha);

    int peak_detect_process(const float* datain,
                            const int datain_length,
                            int* peak_pos,
                            int* peak_max);

    void send_sync_start();

    void derotate(const gr_complex* in, gr_complex* out);

public:
    dvbt_ofdm_sym_acquisition_impl(
        int blocks, int fft_length, int occupied_tones, int cp_length, float snr);
    ~dvbt_ofdm_sym_acquisition_impl() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_OFDM_SYM_ACQUISITION_IMPL_H */
