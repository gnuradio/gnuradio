/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef FASTNOISE_SOURCE_IMPL_H
#define FASTNOISE_SOURCE_IMPL_H

#include <gnuradio/analog/fastnoise_source.h>
#include <gnuradio/random.h>
#include <vector>

namespace gr {
namespace analog {

template <class T>
class fastnoise_source_impl : public fastnoise_source<T>
{
private:
    noise_type_t d_type;
    float d_ampl;
    gr::random d_rng;
    std::vector<T> d_samples;
    uint64_t d_state[2];
    size_t d_bitmask;

public:
    fastnoise_source_impl(noise_type_t type, float ampl, uint64_t seed, size_t samples);
    ~fastnoise_source_impl() override;

    T sample() override;
    T sample_unbiased() override;
    const std::vector<T>& samples() const override;

    void set_type(noise_type_t type) override;
    void set_amplitude(float ampl) override;
    void generate();

    noise_type_t type() const override { return d_type; }
    float amplitude() const override { return d_ampl; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace analog
} /* namespace gr */

#endif /* FASTNOISE_SOURCE_IMPL_H */
