/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
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

#ifndef RPC_SHARED_PTR_SELECTION_H
#define RPC_SHARED_PTR_SELECTION_H

#include <memory>

#include "gnuradio/config.h"
#ifdef GR_RPCSERVER_THRIFT
#include <thrift/version.h>
// to get boost includes, if and only if they're still in Thrift:
#include <thrift/concurrency/Thread.h>
#endif

// select a "shared_ptr" type to use: std:: or boost::
//
// Dear Thrift maintainers, should you read this, here's a haiku:
//
// Version numbers carved in strings
// C Preprocessor can't
// Templates can't
// Sadness


namespace gr {

//! \brief constexpr check for whether a character is a digit
constexpr bool digit(char d) { return !(d < '0' || d > '9'); }

/* \brief Convert C string to a Version magic, constexpr.
 *
 * Converts "5.10.3" to 5·10⁶ + 10·10³+ 3 = 5 010 003
 */
constexpr uint64_t cstr_to_version_magic(const char* cstr, uint64_t magicsofar)
{
    // OK, we go: check for the current character being a zero byte, in that case,
    // return magicsofar
    return *cstr ?
                 // next, check whether a digit, in that case, multiply magicsofar with
                 // 10, and parse on into the sunset
               (digit(*cstr)
                    ? cstr_to_version_magic(cstr + 1, magicsofar * 10 + (*cstr - '0'))
                    :
                    // if it's not a digit, maybe it's a dot? throw
                    ((*cstr) == '.' ?
                                    // it's a dot -> multiply by one hundred, move on
                         cstr_to_version_magic(cstr + 1, magicsofar * 100)
                                    :
                                    // not a dot, not a digit, not an end byte
                         (throw "String is not a x.y.z version string")))
                 : magicsofar;
}

/* \brief Convert three-element version numbers to a single unsigned integer. constexpr.
 *
 * Converts 5, 10, 3 to 5·10⁶ + 10·10³+ 3 = 5 010 003
 */
constexpr uint64_t
version_to_version_magic(unsigned int x, unsigned int y, unsigned int z)
{
    return (uint64_t)x * 1000 * 1000 + y * 1000 + z;
}

/* \brief Thrift-specific version check
 *
 * Evaluates THRIFT_VERSION as string by converting it to a version magic.
 *
 * Then, compares that to 0.11.0, which is the first release where Thrift might use
 * std::shared_ptrs.
 *
 * Then, compares to 0.13.0, which is the release where Thrift only uses
 * std::shared_ptr.
 *
 * If inbetween, do the same check that Thrift does in their 0.11-0.12
 * stdcxx.h headers, minus the redundant MSVC check, which is guaranteed by our MSVC
 * minimum requirement, anyway
 */
constexpr bool thrift_version_uses_boost()
{
#ifndef THRIFT_VERSION
    // if there's no thrift, then that doesn't use boost
    return false;
#else // THRIFT_VERSION
    return (cstr_to_version_magic(THRIFT_VERSION, 0) <
            version_to_version_magic(0, 11, 0)) ||
           (cstr_to_version_magic(THRIFT_VERSION, 0) <
                version_to_version_magic(0, 13, 0) &&
    /* This is the internal check that thrift does in 0.11 and 0.12 to decide whether to
      use std:: or boost::shared_ptr. As soon as we require Thrift >= 0.13.0 this
      #define-based check is to be removed. */
#if defined(BOOST_NO_CXX11_SMART_PTR) || defined(FORCE_BOOST_SMART_PTR)
            true
#else
            false
#endif
           );
#endif // THRIFT_VERSION
}

/* \brief template base class; unspecialized; specializations hold a `t` member that's
 * actually a Thrift-compatible `shared_ptr`.
 */
template <bool use_std, class T>
struct rpc_sptr_impl;

//! \brief template specialization holding a `std::shared_ptr`
template <class T>
struct rpc_sptr_impl<true, T> {
    using t = std::shared_ptr<T>;
};

#ifdef BOOST_SHARED_PTR_HPP_INCLUDED
//! \brief template specialization holding a `boost::shared_ptr`
template <class T>
struct rpc_sptr_impl<false, T> {
    using t = boost::shared_ptr<T>;
};
#endif

/* \brief template class with ::t member type that is std:: or boost::shared_ptr,
 * depending on `thrift_version_uses_boost()`.
 */
template <class T>
struct rpc_sptr {
    using t = typename rpc_sptr_impl<!thrift_version_uses_boost(), T>::t;
};
} // namespace gr
#endif
