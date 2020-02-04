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

#include "correctiq_man_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace blocks {

correctiq_man::sptr correctiq_man::make(float real, float imag)
{
    return gnuradio::get_initial_sptr(new correctiq_man_impl(real, imag));
}

/*
 * The private constructor
 */
correctiq_man_impl::correctiq_man_impl(float real, float imag)
    : gr::sync_block("correctiq_man",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_avg_real(real),
      d_avg_img(imag),
      d_buffer_size(8192),
      d_volk_const_buffer(NULL)
{
    d_k = gr_complex(d_avg_real, d_avg_img);

    set_const_buffer(d_buffer_size);

    message_port_register_in(pmt::mp("set_real"));
    set_msg_handler(pmt::mp("set_real"),
                    [this](pmt::pmt_t msg) { this->handle_real(msg); });
    message_port_register_in(pmt::mp("set_imag"));
    set_msg_handler(pmt::mp("set_imag"),
                    [this](pmt::pmt_t msg) { this->handle_imag(msg); });
}

/*
 * Our virtual destructor.
 */
correctiq_man_impl::~correctiq_man_impl()
{
    if (d_volk_const_buffer)
        volk_free(d_volk_const_buffer);
}

void correctiq_man_impl::set_const_buffer(int new_size)
{
    d_buffer_size = new_size;

    if (d_volk_const_buffer) {
        volk_free(d_volk_const_buffer);
    }

    d_volk_const_buffer = reinterpret_cast<gr_complex*>(
        volk_malloc(sizeof(gr_complex) * d_buffer_size, volk_get_alignment()));

    fill_const_buffer();
}

void correctiq_man_impl::fill_const_buffer()
{
    gr_complex* tmp_ptr = d_volk_const_buffer;

    for (int i = 0; i < d_buffer_size; i++) {
        *tmp_ptr++ = d_k;
    }
}

float correctiq_man_impl::get_real() { return d_avg_real; }
float correctiq_man_impl::get_imag() { return d_avg_img; }

void correctiq_man_impl::set_real(float new_value)
{
    gr::thread::scoped_lock guard(d_setlock);

    d_avg_real = new_value;
    d_k = gr_complex(d_avg_real, d_avg_img);
    fill_const_buffer();
}

void correctiq_man_impl::set_imag(float new_value)
{
    gr::thread::scoped_lock guard(d_setlock);

    d_avg_img = new_value;
    d_k = gr_complex(d_avg_real, d_avg_img);
    fill_const_buffer();
}

void correctiq_man_impl::handle_real(pmt::pmt_t msg)
{
    if (pmt::is_pair(msg)) {
        pmt::pmt_t data = pmt::cdr(msg);
        if (pmt::is_real(data)) {
            float new_value = pmt::to_float(data);

            set_real(new_value);
        } else {
            GR_LOG_WARN(d_logger, "Non-float real value received.  Ignoring.");
        }
    } else {
        if (pmt::is_real(msg)) {
            float new_value = pmt::to_float(msg);

            set_real(new_value);
        } else {
            GR_LOG_WARN(d_logger, "Non-float real value received.  Ignoring.");
        }
    }
}

void correctiq_man_impl::handle_imag(pmt::pmt_t msg)
{
    if (pmt::is_pair(msg)) {
        pmt::pmt_t data = pmt::cdr(msg);
        if (pmt::is_real(data)) {
            float new_value = pmt::to_float(data);

            set_imag(new_value);
        } else {
            GR_LOG_WARN(d_logger, "Non-float imag value received.  Ignoring.");
        }
    } else {
        if (pmt::is_real(msg)) {
            float new_value = pmt::to_float(msg);

            set_imag(new_value);
        } else {
            GR_LOG_WARN(d_logger, "Non-float imag value received.  Ignoring.");
        }
    }
}

int correctiq_man_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_setlock);

    if (noutput_items > d_buffer_size)
        set_const_buffer(noutput_items);

    // Inputs are complex but we're casting as floats to leverage volk
    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];

    volk_32f_x2_add_32f(
        out, in, reinterpret_cast<float*>(d_volk_const_buffer), 2 * noutput_items);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
