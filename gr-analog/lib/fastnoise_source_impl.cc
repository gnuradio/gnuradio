/* -*- c++ -*- */
/*
 * Copyright 2013,2018 Free Software Foundation, Inc.
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

#include "fastnoise_source_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/xoroshiro128p.h>
#include <stdexcept>
#include <vector>

namespace gr {
namespace analog {

template <class T>
typename fastnoise_source<T>::sptr
fastnoise_source<T>::make(noise_type_t type, float ampl, long seed, long samples)
{
    return gnuradio::get_initial_sptr(
        new fastnoise_source_impl<T>(type, ampl, seed, samples));
}

template <>
void fastnoise_source_impl<gr_complex>::generate()
{
    int noutput_items = d_samples.size();
    switch (d_type) {
    case GR_UNIFORM:
        for (int i = 0; i < noutput_items; i++)
            d_samples[i] = gr_complex(d_ampl * ((d_rng.ran1() * 2.0) - 1.0),
                                      d_ampl * ((d_rng.ran1() * 2.0) - 1.0));
        break;

    case GR_GAUSSIAN:
        for (int i = 0; i < noutput_items; i++)
            d_samples[i] = d_ampl * d_rng.rayleigh_complex();
        break;
    default:
        throw std::runtime_error("invalid type");
    }
}

template <class T>
fastnoise_source_impl<T>::fastnoise_source_impl(noise_type_t type,
                                                float ampl,
                                                long seed,
                                                long samples)
    : sync_block("fastnoise_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(T))),
      d_type(type),
      d_ampl(ampl)
{
    d_samples.resize(samples);
    xoroshiro128p_seed(d_state, (uint64_t)seed);
    generate();
}


template <>
fastnoise_source_impl<gr_complex>::fastnoise_source_impl(noise_type_t type,
                                                         float ampl,
                                                         long seed,
                                                         long samples)
    : sync_block("fastnoise_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_type(type),
      d_ampl(ampl / sqrtf(2.0f))
{
    d_samples.resize(samples);
    xoroshiro128p_seed(d_state, (uint64_t)seed);
    generate();
}

template <class T>
fastnoise_source_impl<T>::~fastnoise_source_impl()
{
}

template <class T>
void fastnoise_source_impl<T>::set_type(noise_type_t type)
{
    gr::thread::scoped_lock l(this->d_setlock);
    d_type = type;
    generate();
}

template <class T>
void fastnoise_source_impl<T>::set_amplitude(float ampl)
{
    gr::thread::scoped_lock l(this->d_setlock);
    d_ampl = ampl;
    generate();
}

template <>
void fastnoise_source_impl<gr_complex>::set_amplitude(float ampl)
{
    gr::thread::scoped_lock l(this->d_setlock);
    d_ampl = ampl / sqrtf(2.0f);
    generate();
}


template <class T>
void fastnoise_source_impl<T>::generate()
{
    int noutput_items = d_samples.size();
    switch (d_type) {
    case GR_UNIFORM:
        for (int i = 0; i < noutput_items; i++)
            d_samples[i] = (T)(d_ampl * ((d_rng.ran1() * 2.0) - 1.0));
        break;

    case GR_GAUSSIAN:
        for (int i = 0; i < noutput_items; i++)
            d_samples[i] = (T)(d_ampl * d_rng.gasdev());
        break;

    case GR_LAPLACIAN:
        for (int i = 0; i < noutput_items; i++)
            d_samples[i] = (T)(d_ampl * d_rng.laplacian());
        break;

    case GR_IMPULSE: // FIXME changeable impulse settings
        for (int i = 0; i < noutput_items; i++)
            d_samples[i] = (T)(d_ampl * d_rng.impulse(9));
        break;
    default:
        throw std::runtime_error("invalid type");
    }
}


template <class T>
int fastnoise_source_impl<T>::work(int noutput_items,
                                   gr_vector_const_void_star& input_items,
                                   gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);

    T* out = (T*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        out[i] = sample();
    }

    return noutput_items;
}

template <class T>
T fastnoise_source_impl<T>::sample()
{
    size_t idx = xoroshiro128p_next(d_state) % d_samples.size();
    return d_samples[idx];
}

template <class T>
T fastnoise_source_impl<T>::sample_unbiased()
{
    uint64_t random_int = xoroshiro128p_next(d_state);
    float s = sample();
    return (random_int & (1 << 23)) ? (-s) : s;
}

template <>
gr_complex fastnoise_source_impl<gr_complex>::sample_unbiased()
{
    uint64_t random_int = xoroshiro128p_next(d_state);
    gr_complex s(sample());
    float re = (random_int & (UINT64_C(1) << 23)) ? (-s.real()) : (s.real());
    float im = (random_int & (UINT64_C(1) << 42)) ? (-s.real()) : (s.real());
    return gr_complex(re, im);
}

template <class T>
const std::vector<T>& fastnoise_source_impl<T>::samples() const
{
    return d_samples;
}


template class fastnoise_source<std::int16_t>;
template class fastnoise_source<std::int32_t>;
template class fastnoise_source<float>;
template class fastnoise_source<gr_complex>;
} /* namespace analog */
} /* namespace gr */
