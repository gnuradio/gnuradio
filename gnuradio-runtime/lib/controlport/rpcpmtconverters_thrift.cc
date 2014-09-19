/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include <gnuradio/rpcpmtconverters_thrift.h>
#include "gnuradio_types.h"
#include <iostream>

GNURadio::Knob
rpcpmtconverter::from_pmt(const pmt::pmt_t& knob)
{
  if(pmt::is_real(knob)) {

    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::DOUBLE;
    result.value.__set_a_double(pmt::to_double(knob));
    return result;
  }
  else if(pmt::is_symbol(knob)) {
    std::string value = pmt::symbol_to_string(knob);
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::STRING;
    result.value.__set_a_string(value);
    return result;
  }
  else if(pmt::is_integer(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::LONG;
    result.value.__set_a_long(pmt::to_long(knob));
    return result;
  }
  else if(pmt::is_bool(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::BOOL;
    result.value.__set_a_bool(pmt::to_bool(knob));
    return result;
  }
  else if(pmt::is_uint64(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::LONG;
    result.value.__set_a_long(pmt::to_uint64(knob));
    return result;
  }
  else if(pmt::is_complex(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::COMPLEX;
    std::complex<double> tmp = pmt::to_complex(knob);
    GNURadio::complex cpx;
    cpx.re = tmp.real();
    cpx.im = tmp.imag();
    result.value.__set_a_complex(cpx);
    return result;
  } //TODO Vectors
  else {
    std::cerr << "Error: Don't know how to handle Knob Type (from): " << std::endl; assert(0);}
  return GNURadio::Knob();
}

pmt::pmt_t
rpcpmtconverter::to_pmt(const GNURadio::Knob& knob)
{

  if(knob.type == GNURadio::BaseTypes::BYTE) {
    return pmt::mp(knob.value.a_byte);
  }
  else if(knob.type == GNURadio::BaseTypes::SHORT) {
    return pmt::mp(knob.value.a_short);
  }
  else if(knob.type == GNURadio::BaseTypes::LONG) {
    return pmt::mp(knob.value.a_long);
  }
  else if(knob.type == GNURadio::BaseTypes::DOUBLE) {
    return pmt::mp(knob.value.a_double);
  }
  else if(knob.type == GNURadio::BaseTypes::STRING) {
    return pmt::string_to_symbol(knob.value.a_string);
  }
  else if(knob.type == GNURadio::BaseTypes::BOOL) {
    if (knob.value.a_bool)
      return pmt::PMT_T;
    else
      return pmt::PMT_F;
  }
  else if(knob.type == GNURadio::BaseTypes::COMPLEX) {
    std::complex<double> cpx(knob.value.a_complex.re, knob.value.a_complex.im);
    return pmt::from_complex(cpx);
  }
  else {
    std::cerr << "Error: Don't know how to handle Knob Type: " << knob.type << std::endl; assert(0);
  }
  return pmt::pmt_t();
}
