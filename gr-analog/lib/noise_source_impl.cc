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

#include "noise_source_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace analog {

template <class T>
typename noise_source<T>::sptr noise_source<T>::make(noise_type_t type, float ampl, long seed) {
    return gnuradio::get_initial_sptr(new noise_source_impl<T>(type, ampl, seed));
}

template <class T>
noise_source_impl<T>::noise_source_impl(noise_type_t type, float ampl, long seed)
    : sync_block("noise_source", io_signature::make(0, 0, 0), io_signature::make(1, 1, sizeof(T))),
      d_type(type), d_ampl(ampl), d_rng(seed) {}

template <>
noise_source_impl<gr_complex>::noise_source_impl(noise_type_t type, float ampl, long seed)
    : sync_block("noise_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_type(type), d_ampl(ampl / sqrtf(2.0f)), d_rng(seed) {}


template <class T>
noise_source_impl<T>::~noise_source_impl<T>() {}

template <class T>
void noise_source_impl<T>::set_type(noise_type_t type) {
    gr::thread::scoped_lock l(this->d_setlock);
    d_type = type;
}

template <class T>
void noise_source_impl<T>::set_amplitude(float ampl) {
    gr::thread::scoped_lock l(this->d_setlock);
    d_ampl = ampl;
}

template <>
void noise_source_impl<gr_complex>::set_amplitude(float ampl) {
    gr::thread::scoped_lock l(this->d_setlock);
    d_ampl = ampl / sqrtf(2.0f);
}


template <class T>
int noise_source_impl<T>::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items) {
    gr::thread::scoped_lock l(this->d_setlock);

    T* out = (T*)output_items[0];

    switch (d_type) {
    case GR_UNIFORM:
        for (int i = 0; i < noutput_items; i++) {
            out[i] = (T)(d_ampl * ((d_rng.ran1() * 2.0) - 1.0));
        }
        break;

    case GR_GAUSSIAN:
        for (int i = 0; i < noutput_items; i++) {
            out[i] = (T)(d_ampl * d_rng.gasdev());
        }
        break;

    case GR_LAPLACIAN:
        for (int i = 0; i < noutput_items; i++) {
            out[i] = (T)(d_ampl * d_rng.laplacian());
        }
        break;

    case GR_IMPULSE: // FIXME changeable impulse settings
        for (int i = 0; i < noutput_items; i++) {
            out[i] = (T)(d_ampl * d_rng.impulse(9));
        }
        break;
    default:
        throw std::runtime_error("invalid type");
    }

    return noutput_items;
}

template <>
int noise_source_impl<gr_complex>::work(int noutput_items,
                                        gr_vector_const_void_star& input_items,
                                        gr_vector_void_star& output_items) {
    gr::thread::scoped_lock l(this->d_setlock);

    gr_complex* out = (gr_complex*)output_items[0];

    switch (d_type) {

    case GR_UNIFORM:
        for (int i = 0; i < noutput_items; i++) {
            out[i] = gr_complex(d_ampl * ((d_rng.ran1() * 2.0) - 1.0),
                                d_ampl * ((d_rng.ran1() * 2.0) - 1.0));
        }
        break;

    case GR_GAUSSIAN:
        for (int i = 0; i < noutput_items; i++) {
            out[i] = d_ampl * d_rng.rayleigh_complex();
        }
        break;

    default:
        throw std::runtime_error("invalid type");
    }

    return noutput_items;
}


template class noise_source<std::int16_t>;
template class noise_source<std::int32_t>;
template class noise_source<float>;
template class noise_source<gr_complex>;
} /* namespace analog */
} /* namespace gr */
