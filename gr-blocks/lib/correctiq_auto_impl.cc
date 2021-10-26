/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "correctiq_auto_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

namespace {
constexpr int default_const_buffer = 8192;
}

correctiq_auto::sptr
correctiq_auto::make(double samp_rate, double freq, float gain, float sync_window)
{
    return gnuradio::make_block_sptr<correctiq_auto_impl>(
        samp_rate, freq, gain, sync_window);
}

/*
 * The private constructor
 */
correctiq_auto_impl::correctiq_auto_impl(double samp_rate,
                                         double freq,
                                         float gain,
                                         float sync_window)
    : gr::sync_block("correctiq_auto",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_samp_rate(samp_rate),
      d_freq(freq),
      d_gain(gain),
      d_sync_window(sync_window),
      d_max_sync_samples((long)(d_samp_rate * (double)d_sync_window)),
      d_volk_const_buffer(default_const_buffer)
{
    set_const_buffer(default_const_buffer);

    message_port_register_in(pmt::mp("rsync"));
    set_msg_handler(pmt::mp("rsync"),
                    [this](pmt::pmt_t msg) { this->handle_resync(msg); });

    message_port_register_out(pmt::mp("sync_start"));
    message_port_register_out(pmt::mp("offsets"));

    d_logger->info("Block start.  Auto Synchronizing...");
}

void correctiq_auto_impl::trigger_resync(std::string reason)
{
    gr::thread::scoped_lock guard(d_setlock);

    d_logger->info("{:s}.  Auto Synchronizing...", reason);

    d_synchronized = false;
    d_sync_counter = 0;

    // reset counters on an unsync
    d_avg_real = 0.0;
    d_avg_img = 0.0;
    d_k = gr_complex(d_avg_real, d_avg_img);

    send_syncing();
}

void correctiq_auto_impl::send_sync_values()
{
    pmt::pmt_t meta = pmt::make_dict();

    meta = pmt::dict_add(meta, pmt::mp("real"), pmt::mp(d_avg_real));
    meta = pmt::dict_add(meta, pmt::mp("imag"), pmt::mp(d_avg_img));

    pmt::pmt_t pdu = pmt::cons(meta, pmt::PMT_NIL);
    message_port_pub(pmt::mp("offsets"), pdu);
}

void correctiq_auto_impl::send_syncing()
{
    pmt::pmt_t pdu = pmt::cons(pmt::intern("syncing"), pmt::from_bool(true));

    message_port_pub(pmt::mp("sync_start"), pdu);
}

void correctiq_auto_impl::handle_resync(pmt::pmt_t msg)
{
    trigger_resync("Resync message received.");
}

/*
 * Our virtual destructor.
 */
correctiq_auto_impl::~correctiq_auto_impl() {}

void correctiq_auto_impl::set_const_buffer(int new_size)
{
    d_volk_const_buffer.resize(new_size);
    fill_const_buffer();
}

void correctiq_auto_impl::fill_const_buffer()
{
    std::fill(std::begin(d_volk_const_buffer), std::end(d_volk_const_buffer), d_k);
}

double correctiq_auto_impl::get_freq() const { return d_freq; }

float correctiq_auto_impl::get_gain() const { return d_gain; }

void correctiq_auto_impl::set_freq(double new_value)
{
    d_freq = new_value;
    trigger_resync("Frequency change detected");
}

void correctiq_auto_impl::set_gain(float new_value)
{
    d_gain = new_value;
    trigger_resync("Gain change detected");
}

int correctiq_auto_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_setlock);

    if (!d_synchronized) {
        const gr_complex* in = (const gr_complex*)input_items[0];
        gr_complex* out = (gr_complex*)output_items[0];

        for (int i = 0; i < noutput_items; i++) {
            // Synchronizing.  Behave just like normal correctiq.
            d_avg_real = d_ratio * (in[i].real() - d_avg_real) + d_avg_real;
            d_avg_img = d_ratio * (in[i].imag() - d_avg_img) + d_avg_img;
            d_sync_counter++;

            out[i].real(in[i].real() - d_avg_real);
            out[i].imag(in[i].imag() - d_avg_img);
        }
    } else {
        if (noutput_items > static_cast<int>(d_volk_const_buffer.size()))
            set_const_buffer(noutput_items);

        // Inputs are complex but we're casting as floats to leverage volk
        const float* in = (const float*)input_items[0];
        float* out = (float*)output_items[0];

        volk_32f_x2_subtract_32f(out,
                                 in,
                                 reinterpret_cast<float*>(d_volk_const_buffer.data()),
                                 2 * noutput_items);
    }

    if (!d_synchronized && (d_sync_counter >= d_max_sync_samples)) {
        d_synchronized = true;
        d_k = gr_complex(d_avg_real, d_avg_img);
        fill_const_buffer();

        d_logger->info("Auto offset now synchronized.");
        d_logger->info("Applying these offsets (real-real_offset, imag-imag_offset)... "
                       "real_offset: {:.6f}, imag_offset: {:6f}",
                       d_avg_real,
                       d_avg_img);

        send_sync_values();
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
