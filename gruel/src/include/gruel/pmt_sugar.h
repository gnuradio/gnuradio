/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GRUEL_PMT_SUGAR_H
#define INCLUDED_GRUEL_PMT_SUGAR_H

/*!
 * This file is included by pmt.h and contains pseudo-constructor
 * shorthand for making pmt objects
 */

namespace pmt {

  //! Make pmt symbol
  static inline pmt_t
  mp(const std::string &s)
  {
    return pmt_string_to_symbol(s);
  }

  //! Make pmt symbol
  static inline pmt_t
  mp(const char *s)
  {
    return pmt_string_to_symbol(s);
  }

  //! Make pmt long
  static inline pmt_t
  mp(long x){
    return pmt_from_long(x);
  }

  //! Make pmt long
  static inline pmt_t
  mp(int x){
    return pmt_from_long(x);
  }

  //! Make pmt double
  static inline pmt_t
  mp(double x){
    return pmt_from_double(x);
  }
  
  //! Make pmt complex
  static inline pmt_t
  mp(std::complex<double> z)
  {
    return pmt_make_rectangular(z.real(), z.imag());
  }

  //! Make pmt complex
  static inline pmt_t
  mp(std::complex<float> z)
  {
    return pmt_make_rectangular(z.real(), z.imag());
  }

  //! Make pmt msg_accepter
  static inline pmt_t
  mp(boost::shared_ptr<gruel::msg_accepter> ma)
  {
    return pmt_make_msg_accepter(ma);
  }

  //! Make pmt Binary Large Object (BLOB)
  static inline pmt_t
  mp(const void *data, size_t len_in_bytes)
  {
    return pmt_make_blob(data, len_in_bytes);
  }

} /* namespace pmt */


#endif /* INCLUDED_GRUEL_PMT_SUGAR_H */
