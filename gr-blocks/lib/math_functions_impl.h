/* -*- c++ -*- */
/*
 * Copyright 2020 gnuradio.org.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_MATH_FUNCIONS_IMPL_H
#define INCLUDED_MATH_FUNCIONS_IMPL_H

#include <gnuradio/blocks/math_functions.h>

namespace gr {
namespace blocks {

template <class T, T (*func)(const T&)>
class math_functions_complex_float_impl : public math_functions_complex_float<T, func>
{
public:
    math_functions_complex_float_impl();
    ~math_functions_complex_float_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};


template <class T, double (*func)(double)>
class math_functions_float_impl : public math_functions_float<T, func>
{
public:
    math_functions_float_impl();
    ~math_functions_float_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};


} // namespace blocks
} // namespace gr

#endif /* INCLUDED_MATH_FUNCIONS_IMPL_H */
