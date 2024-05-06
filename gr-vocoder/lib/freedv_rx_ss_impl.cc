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

#include "freedv_rx_ss_impl.h"

#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace vocoder {

freedv_rx_ss::sptr
freedv_rx_ss::make(int mode, float squelch_thresh, int interleave_frames)
{
    return gnuradio::make_block_sptr<freedv_rx_ss_impl>(
        mode, squelch_thresh, interleave_frames);
}

freedv_rx_ss_impl::freedv_rx_ss_impl(int mode,
                                     float squelch_thresh,
                                     int interleave_frames)
    : gr::block("vocoder_freedv_rx_ss",
                io_signature::make(1, 1, sizeof(short)),
                io_signature::make(1, 1, sizeof(short))),
      d_port(pmt::mp("text")),
      d_squelch_thresh(squelch_thresh)
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
        throw std::runtime_error("freedv_rx_ss_impl: freedv_open failed");
#endif
    freedv_set_snr_squelch_thresh(d_freedv, d_squelch_thresh);
    freedv_set_squelch_en(d_freedv, 0);
    freedv_set_callback_txt(d_freedv, put_next_rx_char, NULL, this);
    message_port_register_out(d_port);
    d_speech_samples = freedv_get_n_speech_samples(d_freedv);
    d_max_modem_samples = freedv_get_n_max_modem_samples(d_freedv);
    d_nin = freedv_nin(d_freedv);
    set_output_multiple(d_max_modem_samples);
}

freedv_rx_ss_impl::~freedv_rx_ss_impl()
{
    int total_bits;
    int total_bit_errors;

    if (freedv_get_test_frames(d_freedv)) {
        total_bits = freedv_get_total_bits(d_freedv);
        total_bit_errors = freedv_get_total_bit_errors(d_freedv);
        d_logger->error("bits: {:8d} errors: {:8d} BER: {:3.2f}",
                        total_bits,
                        total_bit_errors,
                        ((1.0 * total_bit_errors) / total_bits));
    }
    freedv_close(d_freedv);
}

void freedv_rx_ss_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    unsigned ninputs = ninput_items_required.size();
    for (unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = std::max(d_nin, noutput_items);
}

int freedv_rx_ss_impl::general_work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    short* in = const_cast<short*>((const short*)input_items[0]);
    short* out = (short*)output_items[0];

    int in_offset = 0, out_offset = 0;

    while ((noutput_items - out_offset) >= d_max_modem_samples &&
           (ninput_items[0] - in_offset) >= d_nin) {
        d_nout = freedv_rx(d_freedv, out + out_offset, in + in_offset);
        out_offset += d_nout;
        in_offset += d_nin;
        d_nin = freedv_nin(d_freedv);
    }

    freedv_get_modem_stats(d_freedv, &d_sync, &d_snr_est);
    freedv_get_modem_extended_stats(d_freedv, &d_stats);
    d_total_bit_errors = freedv_get_total_bit_errors(d_freedv);

    consume_each(in_offset);
    return out_offset;
}

void put_next_rx_proto(void* callback_state, char* proto_bits) { return; }

void datarx(void* callback_state, unsigned char* packet, size_t size) { return; }

void datatx(void* callback_state, unsigned char* packet, size_t* size) { return; }

void freedv_rx_ss_impl::set_squelch_thresh(float squelch_thresh)
{
    gr::thread::scoped_lock l(d_setlock);
    d_squelch_thresh = squelch_thresh;
    freedv_set_snr_squelch_thresh(d_freedv, d_squelch_thresh);
}

void freedv_rx_ss_impl::set_squelch_en(bool squelch_enabled)
{
    gr::thread::scoped_lock l(d_setlock);
    d_squelch_en = squelch_enabled;
    if (squelch_enabled)
        freedv_set_squelch_en(d_freedv, 1);
    else
        freedv_set_squelch_en(d_freedv, 0);
}

float freedv_rx_ss_impl::squelch_thresh() { return (d_squelch_thresh); }

void freedv_rx_ss_impl::put_next_rx_char(void* callback_state, char c)
{
    freedv_rx_ss_impl* instance = static_cast<freedv_rx_ss_impl*>(callback_state);

    if (c == '\r') {
        instance->message_port_pub(instance->d_port, pmt::intern(instance->d_rx_str));
        instance->d_rx_str = "";
    } else {
        instance->d_rx_str += c;
    }
}

} /* namespace vocoder */
} /* namespace gr */
