/* -*- c++ -*- */
/*
 * Copyright 2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "fastnoise_source_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/xoroshiro128p.h>
#include <type_traits>
#include <stdexcept>
#include <vector>

namespace gr {
namespace analog {

bool constexpr is_pwr_of_two(size_t value)
{
    // simple binary trick: an integer x is power of 2 if x-1 is all 1s, but only below
    // the old 1-position.
    // also, zero is not a power of two
    return value && !(value & (value - 1));
}
template <class T>
typename fastnoise_source<T>::sptr
fastnoise_source<T>::make(noise_type_t type, float ampl, uint64_t seed, size_t samples)
{
    return gnuradio::make_block_sptr<fastnoise_source_impl<T>>(type, ampl, seed, samples);
}

template <>
void fastnoise_source_impl<gr_complex>::generate()
{
    size_t noutput_items = d_samples.size();
    if (noutput_items >= 1 << 23) {
        this->d_logger->info("Generating {:d} complex values. This might take a while.",
                             noutput_items);
    }

    switch (d_type) {
    case GR_UNIFORM:
        for (size_t i = 0; i < noutput_items; i++)
            d_samples[i] = gr_complex(d_ampl * ((d_rng.ran1() * 2.0) - 1.0),
                                      d_ampl * ((d_rng.ran1() * 2.0) - 1.0));
        break;

    case GR_GAUSSIAN:
        for (size_t i = 0; i < noutput_items; i++)
            d_samples[i] = d_ampl * d_rng.rayleigh_complex();
        break;
    default:
        throw std::runtime_error("invalid type");
    }
}

template <class T>
fastnoise_source_impl<T>::fastnoise_source_impl(noise_type_t type,
                                                float ampl,
                                                uint64_t seed,
                                                size_t samples)
    : sync_block("fastnoise_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(T))),
      d_type(type),
      d_ampl(ampl),
      d_rng(seed),
      d_bitmask(is_pwr_of_two(samples) ? samples - 1 : 0)
{
    if (!d_bitmask) {
        this->d_logger->info(
            "Using non-power-of-2 sample pool size {:d}. This has negative "
            "effect on performance.",
            samples);
    }
    d_samples.resize(samples);
    xoroshiro128p_seed(d_state, seed);
    this->d_logger->debug("Initializing {:s} pool of size {:d} with seed {:x}",
                          std::is_arithmetic_v<T> ? "arithmetic" : "unknown",
                          samples,
                          seed);
    generate();
}


template <>
fastnoise_source_impl<gr_complex>::fastnoise_source_impl(noise_type_t type,
                                                         float ampl,
                                                         uint64_t seed,
                                                         size_t samples)
    : sync_block("fastnoise_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(gr_complex))),
      d_type(type),
      d_ampl(ampl / sqrtf(2.0f)),
      d_rng(seed),
      d_bitmask(is_pwr_of_two(samples) ? samples - 1 : 0)
{
    if (!d_bitmask) {
        this->d_logger->info(
            "Using non-power-of-2 sample pool size {:d}. This has negative "
            "effect on performance.",
            samples);
    }
    d_samples.resize(samples);
    xoroshiro128p_seed(d_state, seed);
    this->d_logger->debug(
        "Initializing {:s} pool of size {:d} with seed {:x}", "complex", samples, seed);
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
    size_t noutput_items = d_samples.size();
    if (noutput_items >= 1 << 23) {
        this->d_logger->info("Generating {:d} values. This might take a while.",
                             noutput_items);
    }
    switch (d_type) {
    case GR_UNIFORM:
        for (size_t i = 0; i < noutput_items; i++)
            d_samples[i] = (T)(d_ampl * ((d_rng.ran1() * 2.0) - 1.0));
        break;

    case GR_GAUSSIAN:
        for (size_t i = 0; i < noutput_items; i++)
            d_samples[i] = (T)(d_ampl * d_rng.gasdev());
        break;

    case GR_LAPLACIAN:
        for (size_t i = 0; i < noutput_items; i++)
            d_samples[i] = (T)(d_ampl * d_rng.laplacian());
        break;

    case GR_IMPULSE: // FIXME changeable impulse settings
        for (size_t i = 0; i < noutput_items; i++)
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
    size_t idx;
    if (d_bitmask) {
        idx = xoroshiro128p_next(d_state) & d_bitmask;
    } else {
        idx = xoroshiro128p_next(d_state) % d_samples.size();
    }
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
