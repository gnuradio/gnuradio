/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef NOISE_SOURCE_IMPL_H
#define NOISE_SOURCE_IMPL_H

#include <gnuradio/analog/noise_source.h>
#include <gnuradio/random.h>

namespace gr {
namespace analog {

template <class T>
class noise_source_impl : public noise_source<T>
{
    noise_type_t d_type;
    float d_ampl;
    gr::random d_rng;

public:
    noise_source_impl(noise_type_t type, float ampl, long seed = 0);
    ~noise_source_impl();

    void set_type(noise_type_t type);
    void set_amplitude(float ampl);

    noise_type_t type() const { return d_type; }
    float amplitude() const { return d_ampl; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace analog
} /* namespace gr */

#endif /* NOISE_SOURCE_IMPL_H */
