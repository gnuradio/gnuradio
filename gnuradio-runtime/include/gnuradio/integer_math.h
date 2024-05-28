/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_GR_INTEGER_MATH_H
#define INCLUDED_GR_INTEGER_MATH_H

#if (__cplusplus >= 201703L)

// Prefer C++17 goodness.
#include <numeric>
#define GR_GCD std::gcd
#define GR_LCM std::lcm

#elif (BOOST_VERSION >= 105800)

// Fallback: newer boost API (introduced in Boost 1.58.0).
#include <boost/integer/common_factor_rt.hpp>
#define GR_GCD boost::integer::gcd
#define GR_LCM boost::integer::lcm

#else

// Last resort: old deprecated boost API.
#include <boost/math/common_factor_rt.hpp>
#define GR_GCD boost::math::gcd
#define GR_LCM boost::math::lcm

#endif /* __cplusplus >= 201703L */
#endif /* INCLUDED_GR_INTEGER_MATH_H */
