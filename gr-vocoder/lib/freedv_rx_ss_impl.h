/* -*- c++ -*- */
/*
 * Copyright 2016-2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_FREEDV_RX_SS_IMPL_H
#define INCLUDED_VOCODER_FREEDV_RX_SS_IMPL_H

#include <gnuradio/vocoder/freedv_rx_ss.h>

namespace gr {
namespace vocoder {

class freedv_rx_ss_impl : public freedv_rx_ss
{
private:
    struct freedv* d_freedv;
    int d_nin, d_nout;
    std::string d_rx_str;
    static void put_next_rx_char(void* callback_state, char c);
    const pmt::pmt_t d_port;
    struct MODEM_STATS d_stats;
    int d_sync;
    int d_total_bit_errors;
    float d_snr_est;
    float d_squelch_thresh;
    bool d_squelch_en;
    int d_speech_samples;
    int d_max_modem_samples;
#ifdef FREEDV_MODE_700D
    struct freedv_advanced d_adv;
#endif

public:
    freedv_rx_ss_impl(int mode, float squelch_thresh, int interleave_frames);
    ~freedv_rx_ss_impl() override;

    void set_squelch_thresh(float squelch_thresh) override;
    float squelch_thresh() override;
    void set_squelch_en(bool squelch_enabled) override;

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_FREEDV_RX_SS_IMPL_H */
