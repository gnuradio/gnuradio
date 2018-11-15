/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sig_source_impl.h"
#include <gnuradio/gr_complex.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

#include <algorithm>
#include <stdexcept>

namespace gr {
namespace analog {

template <class T>
typename sig_source<T>::sptr sig_source<T>::make(
    double sampling_freq, gr_waveform_t waveform, double frequency, double ampl, T offset, float phase) {
    return gnuradio::get_initial_sptr(
        new sig_source_impl<T>(sampling_freq, waveform, frequency, ampl, offset, phase));
}

template <class T>
sig_source_impl<T>::sig_source_impl(
    double sampling_freq, gr_waveform_t waveform, double frequency, double ampl, T offset, float phase)
    : sync_block("sig_source", io_signature::make(0, 0, 0), io_signature::make(1, 1, sizeof(T))),
      d_sampling_freq(sampling_freq), d_waveform(waveform), d_frequency(frequency), d_ampl(ampl),
      d_offset(offset) {
    this->set_frequency(frequency);
    this->set_phase(phase);
    this->message_port_register_in(pmt::mp("freq"));
    this->set_msg_handler(pmt::mp("freq"), boost::bind(&sig_source_impl<T>::set_frequency_msg, this, _1));
}

template <class T>
sig_source_impl<T>::~sig_source_impl() {}

template <class T>
void sig_source_impl<T>::set_frequency_msg(pmt::pmt_t msg) {
    // Accepts either a number that is assumed to be the new
    // frequency or a key:value pair message where the key must be
    // "freq" and the value is the new frequency.

    if (pmt::is_number(msg)) {
        set_frequency(pmt::to_double(msg));
    } else if (pmt::is_pair(msg)) {
        pmt::pmt_t key = pmt::car(msg);
        pmt::pmt_t val = pmt::cdr(msg);
        if (pmt::eq(key, pmt::intern("freq"))) {
            if (pmt::is_number(val)) {
                set_frequency(pmt::to_double(val));
            }
        } else {
            GR_LOG_WARN(this->d_logger,
                        boost::format("Set Frequency Message must have "
                                      "the key = 'freq'; got '%1%'.") %
                            pmt::write_string(key));
        }
    } else {
        GR_LOG_WARN(this->d_logger,
                    "Set Frequency Message must be either a number or a "
                    "key:value pair where the key is 'freq'.");
    }
}

template <class T>
int sig_source_impl<T>::work(int noutput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items) {
    T* optr = (T*)output_items[0];
    T t;
    gr::thread::scoped_lock l(this->d_setlock);

    switch (d_waveform) {
    case GR_CONST_WAVE:
        t = (T)d_ampl + d_offset;
        std::fill_n(optr, noutput_items, t);
        break;

    case GR_SIN_WAVE:
        d_nco.sin(optr, noutput_items, d_ampl);
        if (d_offset == 0)
            break;

        for (int i = 0; i < noutput_items; i++) {
            optr[i] += d_offset;
        }
        break;

    case GR_COS_WAVE:
        d_nco.cos(optr, noutput_items, d_ampl);
        if (d_offset == 0)
            break;

        for (int i = 0; i < noutput_items; i++) {
            optr[i] += d_offset;
        }
        break;

        /* The square wave is high from -PI to 0. */
    case GR_SQR_WAVE:
        t = (T)d_ampl + d_offset;
        for (int i = 0; i < noutput_items; i++) {
            if (d_nco.get_phase() < 0)
                optr[i] = t;
            else
                optr[i] = d_offset;
            d_nco.step();
        }
        break;

        /* The triangle wave rises from -PI to 0 and falls from 0 to PI. */
    case GR_TRI_WAVE:
        for (int i = 0; i < noutput_items; i++) {
            double t = d_ampl * d_nco.get_phase() / GR_M_PI;
            if (d_nco.get_phase() < 0)
                optr[i] = static_cast<T>(t + d_ampl + d_offset);
            else
                optr[i] = static_cast<T>(-1 * t + d_ampl + d_offset);
            d_nco.step();
        }
        break;

        /* The saw tooth wave rises from -PI to PI. */
    case GR_SAW_WAVE:
        for (int i = 0; i < noutput_items; i++) {
            t = static_cast<T>(d_ampl * d_nco.get_phase() / (2 * GR_M_PI) + d_ampl / 2 + d_offset);
            optr[i] = t;
            d_nco.step();
        }
        break;
    default:
        throw std::runtime_error("analog::sig_source: invalid waveform");
    }

    return noutput_items;
}


template <>
int sig_source_impl<gr_complex>::work(int noutput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items) {
    gr_complex* optr = (gr_complex*)output_items[0];
    gr_complex t;
    gr::thread::scoped_lock l(this->d_setlock);

    switch (d_waveform) {
    case GR_CONST_WAVE:
        t = (gr_complex)d_ampl + d_offset;
        std::fill_n(optr, noutput_items, t);
        break;

    case GR_SIN_WAVE:
    case GR_COS_WAVE:
        d_nco.sincos(optr, noutput_items, d_ampl);
        if (d_offset == gr_complex(0, 0))
            break;

        for (int i = 0; i < noutput_items; i++) {
            optr[i] += d_offset;
        }
        break;

        /* Implements a real square wave high from -PI to 0.
         * The imaginary square wave leads by 90 deg.
         */
    case GR_SQR_WAVE:
        for (int i = 0; i < noutput_items; i++) {
            if (d_nco.get_phase() < -1 * GR_M_PI / 2)
                optr[i] = gr_complex(d_ampl, 0) + d_offset;
            else if (d_nco.get_phase() < 0)
                optr[i] = gr_complex(d_ampl, d_ampl) + d_offset;
            else if (d_nco.get_phase() < GR_M_PI / 2)
                optr[i] = gr_complex(0, d_ampl) + d_offset;
            else
                optr[i] = d_offset;
            d_nco.step();
        }
        break;

        /* Implements a real triangle wave rising from -PI to 0 and
         * falling from 0 to PI. The imaginary triangle wave leads by
         * 90 deg.
         */
    case GR_TRI_WAVE:
        for (int i = 0; i < noutput_items; i++) {
            if (d_nco.get_phase() < -1 * GR_M_PI / 2) {
                optr[i] = gr_complex(d_ampl * d_nco.get_phase() / GR_M_PI + d_ampl,
                                     -1 * d_ampl * d_nco.get_phase() / GR_M_PI - d_ampl / 2) +
                          d_offset;
            } else if (d_nco.get_phase() < 0) {
                optr[i] = gr_complex(d_ampl * d_nco.get_phase() / GR_M_PI + d_ampl,
                                     d_ampl * d_nco.get_phase() / GR_M_PI + d_ampl / 2) +
                          d_offset;
            } else if (d_nco.get_phase() < GR_M_PI / 2) {
                optr[i] = gr_complex(-1 * d_ampl * d_nco.get_phase() / GR_M_PI + d_ampl,
                                     d_ampl * d_nco.get_phase() / GR_M_PI + d_ampl / 2) +
                          d_offset;
            } else {
                optr[i] = gr_complex(-1 * d_ampl * d_nco.get_phase() / GR_M_PI + d_ampl,
                                     -1 * d_ampl * d_nco.get_phase() / GR_M_PI + 3 * d_ampl / 2) +
                          d_offset;
            }
            d_nco.step();
        }
        break;

        /* Implements a real saw tooth wave rising from -PI to PI.
         * The imaginary saw tooth wave leads by 90 deg.
         */
    case GR_SAW_WAVE:
        for (int i = 0; i < noutput_items; i++) {
            if (d_nco.get_phase() < -1 * GR_M_PI / 2) {
                optr[i] = gr_complex(d_ampl * d_nco.get_phase() / (2 * GR_M_PI) + d_ampl / 2,
                                     d_ampl * d_nco.get_phase() / (2 * GR_M_PI) + 5 * d_ampl / 4) +
                          d_offset;
            } else {
                optr[i] = gr_complex(d_ampl * d_nco.get_phase() / (2 * GR_M_PI) + d_ampl / 2,
                                     d_ampl * d_nco.get_phase() / (2 * GR_M_PI) + d_ampl / 4) +
                          d_offset;
            }
            d_nco.step();
        }
        break;
    default:
        throw std::runtime_error("analog::sig_source: invalid waveform");
    }

    return noutput_items;
}


template <class T>
void sig_source_impl<T>::set_sampling_freq(double sampling_freq) {
    d_sampling_freq = sampling_freq;
    d_nco.set_freq(2 * GR_M_PI * this->d_frequency / this->d_sampling_freq);
}

template <class T>
void sig_source_impl<T>::set_waveform(gr_waveform_t waveform) {
    d_waveform = waveform;
}

template <class T>
void sig_source_impl<T>::set_frequency(double frequency) {
    d_frequency = frequency;
    d_nco.set_freq(2 * GR_M_PI * this->d_frequency / this->d_sampling_freq);
}

template <class T>
void sig_source_impl<T>::set_amplitude(double ampl) {
    d_ampl = ampl;
}

template <class T>
void sig_source_impl<T>::set_offset(T offset) {
    d_offset = offset;
}

template <class T>
void sig_source_impl<T>::set_phase(float phase) {
    gr::thread::scoped_lock l(this->d_setlock);
    d_nco.set_phase(phase);
}


template class sig_source<std::int16_t>;
template class sig_source<std::int32_t>;
template class sig_source<float>;
template class sig_source<gr_complex>;
} /* namespace analog */
} /* namespace gr */
