/* -*- c++ -*- */
/*
 * Copyright 2020 gnuradio.org.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_MATH_FUNCTIONS_H
#define INCLUDED_MATH_FUNCTIONS_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>


namespace gr {
namespace blocks {


/*!
 * \brief Math functions from std::complex<float> family.
 * \ingroup blocks
 *
 */
template <class T, T (*func)(const T&)>
class BLOCKS_API math_functions_complex_float : virtual public sync_block
{
public:
    typedef std::shared_ptr<math_functions_complex_float<T, func>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of
     * blocks::math_functions_complex_float. This is a templated block for using one of
     * below functions in complex<float> domain:
     *  - cos
     *  - cosh
     *  - exp
     *  - log
     *  - sin
     *  - sinh
     *  - sqrt
     *  - tan
     *  - tanh
     *  - acos
     *  - acosh
     *  - asin
     *  - asinh
     *  - atan
     *  - atanh
     */
    static sptr make();
};

// define types needed for blocks.*.yml templages to select propper class
typedef math_functions_complex_float<std::complex<float>, std::cos>
    math_functions_cos_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::cosh>
    math_functions_cosh_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::exp>
    math_functions_exp_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::log>
    math_functions_log_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::sin>
    math_functions_sin_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::sinh>
    math_functions_sinh_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::sqrt>
    math_functions_sqrt_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::tan>
    math_functions_tan_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::tanh>
    math_functions_tanh_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::acos>
    math_functions_acos_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::acosh>
    math_functions_acosh_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::asin>
    math_functions_asin_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::asinh>
    math_functions_asinh_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::atan>
    math_functions_atan_complex_float;
typedef math_functions_complex_float<std::complex<float>, std::atanh>
    math_functions_atanh_complex_float;


/*!
 * \brief Math functions from float family.
 * \ingroup blocks
 *
 */
template <class T, double (*func)(double)>
class BLOCKS_API math_functions_float : virtual public sync_block
{
public:
    typedef std::shared_ptr<math_functions_float<T, func>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of blocks::math_functions_float.
     * This is a templated block for using one of below functions in float domain:
     *  - cos
     *  - cosh
     *  - exp
     *  - log
     *  - sin
     *  - sinh
     *  - sqrt
     *  - tan
     *  - tanh
     *  - acos
     *  - acosh
     *  - asin
     *  - asinh
     *  - atan
     *  - atanh
     */
    static sptr make();
};

// define types needed for blocks.*.yml templages to select propper class
typedef math_functions_float<float, cos> math_functions_cos_float;
typedef math_functions_float<float, cosh> math_functions_cosh_float;
typedef math_functions_float<float, exp> math_functions_exp_float;
typedef math_functions_float<float, log> math_functions_log_float;
typedef math_functions_float<float, sin> math_functions_sin_float;
typedef math_functions_float<float, sinh> math_functions_sinh_float;
typedef math_functions_float<float, sqrt> math_functions_sqrt_float;
typedef math_functions_float<float, tan> math_functions_tan_float;
typedef math_functions_float<float, tanh> math_functions_tanh_float;
typedef math_functions_float<float, acos> math_functions_acos_float;
typedef math_functions_float<float, acosh> math_functions_acosh_float;
typedef math_functions_float<float, asin> math_functions_asin_float;
typedef math_functions_float<float, asinh> math_functions_asinh_float;
typedef math_functions_float<float, atan> math_functions_atan_float;
typedef math_functions_float<float, atanh> math_functions_atanh_float;


} // namespace blocks
} // namespace gr

#endif /* INCLUDED_MATH_FUNCTIONS_H */
