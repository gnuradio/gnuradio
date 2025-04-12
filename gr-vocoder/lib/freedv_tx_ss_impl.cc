/* -*- c++ -*- */
/*
 * Copyright 2016-2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "freedv_tx_ss_impl.h"

#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace vocoder {

freedv_tx_ss::sptr
freedv_tx_ss::make(int mode, const std::string msg_txt, int interleave_frames)
{
    return gnuradio::make_block_sptr<freedv_tx_ss_impl>(mode, msg_txt, interleave_frames);
}

freedv_tx_ss_impl::freedv_tx_ss_impl(int mode,
                                     const std::string msg_txt,
                                     int interleave_frames)
    : block("vocoder_freedv_tx_ss",
            io_signature::make(1, 1, sizeof(short)),
            io_signature::make(1, 1, sizeof(short))),
      d_msg_text(msg_txt)
{
#ifdef FREEDV_MODE_700D
    if (mode == FREEDV_MODE_700D) {
        d_adv.interleave_frames = interleave_frames;
        if ((d_freedv = freedv_open_advanced(mode, &d_adv)) == NULL)
            throw std::runtime_error("freedv_tx_ss_impl: freedv_open_advanced failed");
    } else {
        if ((d_freedv = freedv_open(mode)) == NULL)
            throw std::runtime_error("freedv_tx_ss_impl: freedv_open failed");
    }
#else
    if ((d_freedv = freedv_open(mode)) == NULL)
        throw std::runtime_error("freedv_tx_ss_impl: freedv_open failed");
#endif
    d_tx_str = msg_txt + "\r"; // FreeDV uses Carriage Return termination
    freedv_set_callback_txt(d_freedv, NULL, get_next_tx_char, this);
    d_speech_samples = freedv_get_n_speech_samples(d_freedv);
    d_nom_modem_samples = freedv_get_n_nom_modem_samples(d_freedv);
    set_output_multiple(d_nom_modem_samples);
}

freedv_tx_ss_impl::~freedv_tx_ss_impl() { freedv_close(d_freedv); }

void freedv_tx_ss_impl::set_clip(int val)
{
    d_clip = val;
#if defined(FREEDV_MODE_700C) || defined(FREEDV_MODE_700D)
    freedv_set_clip(d_freedv, d_clip);
#endif
}

void freedv_tx_ss_impl::set_clip(bool val)
{
    if (val)
        freedv_tx_ss_impl::set_clip(1);
    else
        freedv_tx_ss_impl::set_clip(0);
}

void freedv_tx_ss_impl::set_tx_bpf(int val)
{
    d_tx_bpf = val;
#ifdef FREEDV_MODE_700D
    freedv_set_tx_bpf(d_freedv, d_tx_bpf);
#endif
}

void freedv_tx_ss_impl::set_tx_bpf(bool val)
{
    if (val)
        freedv_tx_ss_impl::set_tx_bpf(1);
    else
        freedv_tx_ss_impl::set_tx_bpf(0);
}

void freedv_tx_ss_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = (noutput_items / d_nom_modem_samples) * d_speech_samples;
}

int freedv_tx_ss_impl::general_work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    short* in = const_cast<short*>((const short*)input_items[0]);
    short* out = (short*)output_items[0];
    int i;

    for (i = 0; i < (noutput_items / d_nom_modem_samples); i++)
        freedv_tx(d_freedv, &(out[i * d_nom_modem_samples]), &(in[i * d_speech_samples]));
    consume_each(i * d_speech_samples);
    return noutput_items;
}

char freedv_tx_ss_impl::get_next_tx_char(void* callback_state)
{
    freedv_tx_ss_impl* instance = static_cast<freedv_tx_ss_impl*>(callback_state);
    char c = instance->d_tx_str[instance->d_tx_str_offset++];

    if (instance->d_tx_str_offset == instance->d_tx_str.length())
        instance->d_tx_str_offset = 0;

    return c;
}

} /* namespace vocoder */
} /* namespace gr */
