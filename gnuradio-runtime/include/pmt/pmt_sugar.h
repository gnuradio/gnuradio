/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PMT_SUGAR_H
#define INCLUDED_PMT_SUGAR_H

/*!
 * This file is included by pmt.h and contains pseudo-constructor
 * shorthand for making pmt objects
 */

#include <gnuradio/messages/msg_accepter.h>
#include <string_view>

namespace pmt {

//! Make pmt symbol
static inline pmt_t mp(std::string_view s) { return string_to_symbol(s); }

//! Make pmt symbol
static inline pmt_t mp(const char* s) { return string_to_symbol(s); }

//! Make pmt long
static inline pmt_t mp(long x) { return from_long(x); }

//! Make pmt uint64
static inline pmt_t mp(long unsigned x) { return from_uint64(x); }

//! Make pmt uint64
static inline pmt_t mp(long long unsigned x) { return from_uint64(x); }

//! Make pmt long
static inline pmt_t mp(int x) { return from_long(x); }

//! Make pmt double
static inline pmt_t mp(double x) { return from_double(x); }

//! Make pmt complex
static inline pmt_t mp(std::complex<double> z)
{
    return make_rectangular(z.real(), z.imag());
}

//! Make pmt complex
static inline pmt_t mp(std::complex<float> z)
{
    return make_rectangular(z.real(), z.imag());
}

//! Make pmt msg_accepter
static inline pmt_t mp(std::shared_ptr<gr::messages::msg_accepter> ma)
{
    return make_msg_accepter(ma);
}

//! Make pmt Binary Large Object (BLOB)
static inline pmt_t mp(const void* data, size_t len_in_bytes)
{
    return make_blob(data, len_in_bytes);
}

//! Make tuple
static inline pmt_t mp(const pmt_t& e0) { return make_tuple(e0); }

//! Make tuple
static inline pmt_t mp(const pmt_t& e0, const pmt_t& e1) { return make_tuple(e0, e1); }

//! Make tuple
static inline pmt_t mp(const pmt_t& e0, const pmt_t& e1, const pmt_t& e2)
{
    return make_tuple(e0, e1, e2);
}

//! Make tuple
static inline pmt_t mp(const pmt_t& e0, const pmt_t& e1, const pmt_t& e2, const pmt_t& e3)
{
    return make_tuple(e0, e1, e2, e3);
}

//! Make tuple
static inline pmt_t
mp(const pmt_t& e0, const pmt_t& e1, const pmt_t& e2, const pmt_t& e3, const pmt_t& e4)
{
    return make_tuple(e0, e1, e2, e3, e4);
}

//! Make tuple
static inline pmt_t mp(const pmt_t& e0,
                       const pmt_t& e1,
                       const pmt_t& e2,
                       const pmt_t& e3,
                       const pmt_t& e4,
                       const pmt_t& e5)
{
    return make_tuple(e0, e1, e2, e3, e4, e5);
}

//! Make tuple
static inline pmt_t mp(const pmt_t& e0,
                       const pmt_t& e1,
                       const pmt_t& e2,
                       const pmt_t& e3,
                       const pmt_t& e4,
                       const pmt_t& e5,
                       const pmt_t& e6)
{
    return make_tuple(e0, e1, e2, e3, e4, e5, e6);
}

//! Make tuple
static inline pmt_t mp(const pmt_t& e0,
                       const pmt_t& e1,
                       const pmt_t& e2,
                       const pmt_t& e3,
                       const pmt_t& e4,
                       const pmt_t& e5,
                       const pmt_t& e6,
                       const pmt_t& e7)
{
    return make_tuple(e0, e1, e2, e3, e4, e5, e6, e7);
}

//! Make tuple
static inline pmt_t mp(const pmt_t& e0,
                       const pmt_t& e1,
                       const pmt_t& e2,
                       const pmt_t& e3,
                       const pmt_t& e4,
                       const pmt_t& e5,
                       const pmt_t& e6,
                       const pmt_t& e7,
                       const pmt_t& e8)
{
    return make_tuple(e0, e1, e2, e3, e4, e5, e6, e7, e8);
}

//! Make tuple
static inline pmt_t mp(const pmt_t& e0,
                       const pmt_t& e1,
                       const pmt_t& e2,
                       const pmt_t& e3,
                       const pmt_t& e4,
                       const pmt_t& e5,
                       const pmt_t& e6,
                       const pmt_t& e7,
                       const pmt_t& e8,
                       const pmt_t& e9)
{
    return make_tuple(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9);
}


} /* namespace pmt */


#endif /* INCLUDED_PMT_SUGAR_H */
