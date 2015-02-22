/*
 * Copyright 2014,2015 Free Software Foundation, Inc.
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
#include <gnuradio/gr_complex.h>
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
  }
  else if(pmt::is_f32vector(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::F32VECTOR;
    size_t size(pmt::length(knob));
    const float* start((const float*)pmt::f32vector_elements(knob,size));
    result.value.__set_a_f32vector(std::vector<double>(start,start+size));
    return result;
  }
  else if(pmt::is_f64vector(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::F64VECTOR;
    size_t size(pmt::length(knob));
    const double* start((const double*)pmt::f64vector_elements(knob,size));
    result.value.__set_a_f64vector(std::vector<double>(start,start+size));
    return result;
  }
  else if(pmt::is_s64vector(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::S64VECTOR;
    size_t size(pmt::length(knob));
    const int64_t* start((const int64_t*)pmt::s64vector_elements(knob,size));
    result.value.__set_a_s64vector(std::vector<int64_t>(start,start+size));
    return result;
  }
  else if(pmt::is_s32vector(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::S32VECTOR;
    size_t size(pmt::length(knob));
    const int32_t* start((const int32_t*)pmt::s32vector_elements(knob,size));
    result.value.__set_a_s32vector(std::vector<int32_t>(start,start+size));
    return result;
  }
  else if(pmt::is_s16vector(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::S16VECTOR;
    size_t size(pmt::length(knob));
    const int16_t* start((const int16_t*)pmt::s16vector_elements(knob,size));
    result.value.__set_a_s16vector(std::vector<int16_t>(start,start+size));
    return result;
  }
  else if(pmt::is_s8vector(knob)) {
    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::S8VECTOR;
    size_t size(pmt::length(knob));
    const int8_t* start((const int8_t*)pmt::s8vector_elements(knob,size));
    result.value.__set_a_s8vector(std::vector<int8_t>(start,start+size));
    return result;
  }
  else if(pmt::is_c32vector(knob)) {
    std::vector< GNURadio::complex > z;

    GNURadio::Knob result;
    result.type = GNURadio::BaseTypes::C32VECTOR;
    size_t size(pmt::length(knob));
    const gr_complex* start((const gr_complex*)pmt::c32vector_elements(knob,size));
    for(size_t s = 0; s < size; s++) {
      GNURadio::complex z0;
      gr_complex z1 = gr_complex(*(start+s));
      z0.__set_re(z1.real());
      z0.__set_im(z1.imag());
      z.push_back(z0);
    }
    result.value.__set_a_c32vector(z);
    return result;
  }
  else {
    std::cerr << "Error: Don't know how to handle Knob Type (from): " << knob << std::endl;
    assert(0);
  }
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
  else if(knob.type == GNURadio::BaseTypes::INT) {
    return pmt::mp(knob.value.a_int);
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
    gr_complexd cpx(knob.value.a_complex.re, knob.value.a_complex.im);
    return pmt::from_complex(cpx);
  }
  else if(knob.type == GNURadio::BaseTypes::F32VECTOR) {
    if (knob.value.a_bool)
      return pmt::PMT_T;
    else
      return pmt::PMT_F;
  }
  else {
    std::cerr << "Error: Don't know how to handle Knob Type: " << knob.type << std::endl; assert(0);
  }
  return pmt::pmt_t();
}
