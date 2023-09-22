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

namespace {
constexpr int default_const_buffer = 8192;
}

correctiq_man::sptr correctiq_man::make(float real, float imag)
{
    return gnuradio::make_block_sptr<correctiq_man_impl>(real, imag);
}

/*
 * The private constructor
 */
correctiq_man_impl::correctiq_man_impl(float real, float imag)
    : gr::sync_block("correctiq_man",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_k(real, imag),
      d_volk_const_buffer(default_const_buffer)
{
    set_const_buffer(default_const_buffer);

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
correctiq_man_impl::~correctiq_man_impl() {}

void correctiq_man_impl::set_const_buffer(int new_size)
{
    d_volk_const_buffer.resize(new_size);
    fill_const_buffer();
}

void correctiq_man_impl::fill_const_buffer()
{
    std::fill(std::begin(d_volk_const_buffer), std::end(d_volk_const_buffer), d_k);
}

float correctiq_man_impl::get_real() const { return d_k.real(); }
float correctiq_man_impl::get_imag() const { return d_k.imag(); }

void correctiq_man_impl::set_real(float real)
{
    gr::thread::scoped_lock guard(d_setlock);

    d_k = gr_complex(real, d_k.imag());
    fill_const_buffer();
}

void correctiq_man_impl::set_imag(float imag)
{
    gr::thread::scoped_lock guard(d_setlock);

    d_k = gr_complex(d_k.real(), imag);
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
            d_logger->warn("Non-float real value received.  Ignoring. (In PMT pair.)");
        }
    } else {
        if (pmt::is_real(msg)) {
            float new_value = pmt::to_float(msg);

            set_real(new_value);
        } else {
            d_logger->warn("Non-float real value received.  Ignoring.");
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
            d_logger->warn("Non-float imag value received.  Ignoring. (In PMT pair.)");
        }
    } else {
        if (pmt::is_real(msg)) {
            float new_value = pmt::to_float(msg);

            set_imag(new_value);
        } else {
            d_logger->warn("Non-float imag value received.  Ignoring.");
        }
    }
}

int correctiq_man_impl::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock guard(d_setlock);

    if (noutput_items > static_cast<int>(d_volk_const_buffer.size()))
        set_const_buffer(noutput_items);

    // Inputs are complex but we're casting as floats to leverage volk
    const float* in = (const float*)input_items[0];
    float* out = (float*)output_items[0];

    volk_32f_x2_add_32f(
        out, in, reinterpret_cast<float*>(d_volk_const_buffer.data()), 2 * noutput_items);

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
