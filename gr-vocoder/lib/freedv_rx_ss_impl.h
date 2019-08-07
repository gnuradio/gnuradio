/* -*- c++ -*- */
/*
 * Copyright 2016-2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_VOCODER_FREEDV_RX_SS_IMPL_H
#define INCLUDED_VOCODER_FREEDV_RX_SS_IMPL_H

#include <gnuradio/vocoder/freedv_rx_ss.h>

extern "C" {
struct freedv_rx_callback_state {
    FILE* ftxt;
};
static void put_next_rx_char(void* callback_state, char c);
void put_next_rx_proto(void* callback_state, char* proto_bits);
void datarx(void* callback_state, unsigned char* packet, size_t size);
void datatx(void* callback_state, unsigned char* packet, size_t* size);
}

namespace gr {
namespace vocoder {

class freedv_rx_ss_impl : public freedv_rx_ss
{
private:
    short* d_speech_out;
    short* d_demod_in;
    struct freedv* d_freedv;
    int d_nin, d_nout, d_frame;
    struct freedv_rx_callback_state d_cb_state;
    struct MODEM_STATS d_stats;
    int d_mode;
    int d_sync;
    int d_total_bits;
    int d_total_bit_errors;
    float d_snr_est;
    float d_squelch_thresh;
    bool d_squelch_en;
    int d_speech_samples;
    int d_max_modem_samples;
    float d_clock_offset;
    int d_use_codecrx;
    int d_interleave_frames;
#ifdef FREEDV_MODE_700D
    struct freedv_advanced d_adv;
#endif
    struct CODEC2* d_c2 = NULL;

public:
    freedv_rx_ss_impl(int mode, float squelch_thresh, int interleave_frames);
    ~freedv_rx_ss_impl();

    void set_squelch_thresh(float squelch_thresh);
    float squelch_thresh();
    void set_squelch_en(bool squelch_enabled);

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_FREEDV_RX_SS_IMPL_H */
