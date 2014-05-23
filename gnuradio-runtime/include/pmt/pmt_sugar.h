/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_PMT_SUGAR_H
#define INCLUDED_PMT_SUGAR_H

/*!
 * This file is included by pmt.h and contains pseudo-constructor
 * shorthand for making pmt objects
 */

#include <gnuradio/messages/msg_accepter.h>

namespace pmt {

  //! Make pmt symbol
  static inline pmt_t
  mp(const std::string &s)
  {
    return string_to_symbol(s);
  }

  //! Make pmt symbol
  static inline pmt_t
  mp(const char *s)
  {
    return string_to_symbol(s);
  }

  //! Make pmt long
  static inline pmt_t
  mp(long x){
    return from_long(x);
  }

  //! Make pmt uint64
  static inline pmt_t
  mp(long unsigned x){
    return from_uint64(x);
  }

  //! Make pmt uint64
  static inline pmt_t
  mp(long long unsigned x){
    return from_uint64(x);
  }

  //! Make pmt long
  static inline pmt_t
  mp(int x){
    return from_long(x);
  }

  //! Make pmt double
  static inline pmt_t
  mp(double x){
    return from_double(x);
  }

  //! Make pmt complex
  static inline pmt_t
  mp(std::complex<double> z)
  {
    return make_rectangular(z.real(), z.imag());
  }

  //! Make pmt complex
  static inline pmt_t
  mp(std::complex<float> z)
  {
    return make_rectangular(z.real(), z.imag());
  }

  //! Make pmt msg_accepter
  static inline pmt_t
  mp(boost::shared_ptr<gr::messages::msg_accepter> ma)
  {
    return make_msg_accepter(ma);
  }

  //! Make pmt Binary Large Object (BLOB)
  static inline pmt_t
  mp(const void *data, size_t len_in_bytes)
  {
    return make_blob(data, len_in_bytes);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0)
  {
    return make_tuple(e0);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0, const pmt_t &e1)
  {
    return make_tuple(e0, e1);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2)
  {
    return make_tuple(e0, e1, e2);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3)
  {
    return make_tuple(e0, e1, e2, e3);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4)
  {
    return make_tuple(e0, e1, e2, e3, e4);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5)
  {
    return make_tuple(e0, e1, e2, e3, e4, e5);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6)
  {
    return make_tuple(e0, e1, e2, e3, e4, e5, e6);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7)
  {
    return make_tuple(e0, e1, e2, e3, e4, e5, e6, e7);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8)
  {
    return make_tuple(e0, e1, e2, e3, e4, e5, e6, e7, e8);
  }

  //! Make tuple
  static inline pmt_t
  mp(const pmt_t &e0, const pmt_t &e1, const pmt_t &e2, const pmt_t &e3, const pmt_t &e4, const pmt_t &e5, const pmt_t &e6, const pmt_t &e7, const pmt_t &e8, const pmt_t &e9)
  {
    return make_tuple(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9);
  }


} /* namespace pmt */


#endif /* INCLUDED_PMT_SUGAR_H */
