/* -*- c++ -*- */
/*
 * Copyright 2016-2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_FREEDV_TX_SS_IMPL_H
#define INCLUDED_VOCODER_FREEDV_TX_SS_IMPL_H

#include <gnuradio/vocoder/freedv_tx_ss.h>

namespace gr {
namespace vocoder {

class freedv_tx_ss_impl : public freedv_tx_ss
{
private:
    std::string d_tx_str;
    std::string::size_type d_tx_str_offset = 0;
    static char get_next_tx_char(void* callback_state);
    struct freedv* d_freedv;
    std::string d_msg_text;
    int d_speech_samples;
    int d_nom_modem_samples;
    int d_clip;
    int d_tx_bpf;
#ifdef FREEDV_MODE_700D
    struct freedv_advanced d_adv;
#endif

public:
    freedv_tx_ss_impl(int mode, const std::string txt_msg, int interleave_frames);
    ~freedv_tx_ss_impl() override;

    void set_clip(bool val);
    void set_clip(int val);
    void set_tx_bpf(bool val);
    void set_tx_bpf(int val);

    // Where all the action really happens
    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_FREEDV_TX_SS_IMPL_H */
