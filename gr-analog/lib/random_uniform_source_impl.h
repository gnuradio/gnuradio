/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef RANDOM_UNIFORM_SOURCE_IMPL_H
#define RANDOM_UNIFORM_SOURCE_IMPL_H

#include <gnuradio/analog/random_uniform_source.h>
#include <gnuradio/random.h>

namespace gr {
namespace analog {

template <class T>
class random_uniform_source_impl : public random_uniform_source<T>
{
private:
    gr::random d_rng;

public:
    random_uniform_source_impl(int minimum, int maximum, int seed);
    ~random_uniform_source_impl() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    int random_value();
};

} // namespace analog
} /* namespace gr */

#endif /* RANDOM_UNIFORM_SOURCE_IMPL_H */
