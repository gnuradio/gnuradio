/* -*- c++ -*- */
/*
 * Copyright 2020 gnuradio.org.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "math_functions_impl.h"
#include <gnuradio/io_signature.h>
#include <cmath>
#include <complex>

namespace gr {
namespace blocks {

template <class T, T (*func)(const T&)>
typename math_functions_complex_float<T, func>::sptr
math_functions_complex_float<T, func>::make()
{
    return gnuradio::get_initial_sptr(new math_functions_complex_float_impl<T, func>());
}

/*
 * The private constructor for complex<float> implementation.
 */
template <class T, T (*func)(const T&)>
math_functions_complex_float_impl<T, func>::math_functions_complex_float_impl()
    : gr::sync_block("math_functions_complex_float<T>",
                     gr::io_signature::make(1, 1, sizeof(T)),
                     gr::io_signature::make(1, 1, sizeof(T)))
{
}

/*
 * Virtual destructor for complex<float> implementation.
 */
template <class T, T (*func)(const T&)>
math_functions_complex_float_impl<T, func>::~math_functions_complex_float_impl()
{
}

template <class T, T (*func)(const T&)>
int math_functions_complex_float_impl<T, func>::work(
    int noutput_items,
    gr_vector_const_void_star& input_items,
    gr_vector_void_star& output_items)
{
    const T* in = (const T*)input_items[0];
    T* out = (T*)output_items[0];

    for (size_t i = 0; i < size_t(noutput_items); i++)
        out[i] = func(in[i]);

    return noutput_items;
}

// generate complex<float> implementation code
template class math_functions_complex_float<std::complex<float>, std::cos>;
template class math_functions_complex_float<std::complex<float>, std::cosh>;
template class math_functions_complex_float<std::complex<float>, std::exp>;
template class math_functions_complex_float<std::complex<float>, std::log>;
template class math_functions_complex_float<std::complex<float>, std::sin>;
template class math_functions_complex_float<std::complex<float>, std::sinh>;
template class math_functions_complex_float<std::complex<float>, std::sqrt>;
template class math_functions_complex_float<std::complex<float>, std::tan>;
template class math_functions_complex_float<std::complex<float>, std::tanh>;
template class math_functions_complex_float<std::complex<float>, std::acos>;
template class math_functions_complex_float<std::complex<float>, std::acosh>;
template class math_functions_complex_float<std::complex<float>, std::asin>;
template class math_functions_complex_float<std::complex<float>, std::asinh>;
template class math_functions_complex_float<std::complex<float>, std::atan>;
template class math_functions_complex_float<std::complex<float>, std::atanh>;


template <class T, double (*func)(double)>
typename math_functions_float<T, func>::sptr math_functions_float<T, func>::make()
{
    return gnuradio::get_initial_sptr(new math_functions_float_impl<T, func>());
}

/*
 * The private constructor for float implementation.
 */
template <class T, double (*func)(double)>
math_functions_float_impl<T, func>::math_functions_float_impl()
    : gr::sync_block("math_functions_float<T>",
                     gr::io_signature::make(1, 1, sizeof(T)),
                     gr::io_signature::make(1, 1, sizeof(T)))
{
}

/*
 * Virtual destructor for float implementation.
 */
template <class T, double (*func)(double)>
math_functions_float_impl<T, func>::~math_functions_float_impl()
{
}

template <class T, double (*func)(double)>
int math_functions_float_impl<T, func>::work(int noutput_items,
                                             gr_vector_const_void_star& input_items,
                                             gr_vector_void_star& output_items)
{
    const T* in = (const T*)input_items[0];
    T* out = (T*)output_items[0];

    for (size_t i = 0; i < size_t(noutput_items); i++)
        // Math fuctions take double and return double, but to wokr with floats
        // we need down-cast them.
        out[i] = static_cast<T>(func(static_cast<T>(in[i])));

    return noutput_items;
}

// generate float implementation code
template class math_functions_float<float, cos>;
template class math_functions_float<float, cosh>;
template class math_functions_float<float, exp>;
template class math_functions_float<float, log>;
template class math_functions_float<float, sin>;
template class math_functions_float<float, sinh>;
template class math_functions_float<float, sqrt>;
template class math_functions_float<float, tan>;
template class math_functions_float<float, tanh>;
template class math_functions_float<float, acos>;
template class math_functions_float<float, acosh>;
template class math_functions_float<float, asin>;
template class math_functions_float<float, asinh>;
template class math_functions_float<float, atan>;
template class math_functions_float<float, atanh>;


} // namespace blocks
} /* namespace gr */
