/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gnuradio/rpcpmtconverters_ice.h>
#include <Ice/Ice.h>
#include <gnuradio.h>

GNURadio::KnobPtr
rpcpmtconverter::from_pmt(const pmt::pmt_t& knob, const Ice::Current& c)
{
  if(pmt::is_real(knob)) {
    return new GNURadio::KnobD(Ice::Double(pmt::to_double(knob)));
  }
  else if(pmt::is_symbol(knob)) {
    std::string stuff = pmt::symbol_to_string(knob);
    return new GNURadio::KnobS(stuff);
  }
  else if(pmt::is_integer(knob)) {
    return new GNURadio::KnobI(pmt::to_long(knob));
  }
  else if(pmt::is_bool(knob)) {
    return new GNURadio::KnobB(pmt::to_bool(knob));
  }
  else if(pmt::is_uint64(knob)) {
    return new GNURadio::KnobL(pmt::to_uint64(knob));
  }
  else if(pmt::is_complex(knob)) {
    std::complex<double> tmp = pmt::to_complex(knob);
    GNURadio::complex cpx;
    cpx.re = tmp.real();
    cpx.im = tmp.imag();
    return new GNURadio::KnobZ(cpx);
  }
  else if(pmt::is_c32vector(knob)) {  // c32 sent as interleaved floats
    size_t size(pmt::length(knob));
    const float* start((const float*) pmt::c32vector_elements(knob,size));
    return new GNURadio::KnobVecF(std::vector<float>(start,start+size*2));
  } 
  else if (pmt::is_s32vector(knob)) {
    size_t size(pmt::length(knob));
    const int* start((const int*) pmt::s32vector_elements(knob,size));
    return new GNURadio::KnobVecI(std::vector<int>(start,start+size));
  }
  else if (pmt::is_s16vector(knob)) {
    size_t size(pmt::length(knob));
    const short* start((const short*) pmt::s16vector_elements(knob,size));
    return new GNURadio::KnobVecT(std::vector<short>(start,start+size));
  }
  else if(pmt::is_f32vector(knob)) {
    size_t size(pmt::length(knob));
    const float* start((const float*) pmt::f32vector_elements(knob,size));
    return new GNURadio::KnobVecF(std::vector<float>(start,start+size));
  } 
  else if (pmt::is_u8vector(knob)) {
    size_t size(pmt::length(knob));
    const uint8_t* start((const uint8_t*) pmt::u8vector_elements(knob,size));
    return new GNURadio::KnobVecC(std::vector<Ice::Byte>(start,start+size));
  }
  else if (pmt::is_s8vector(knob)) {
    size_t size(pmt::length(knob));
    const int8_t* start((const int8_t*) pmt::s8vector_elements(knob,size));
    return new GNURadio::KnobVecC(std::vector<Ice::Byte>(start,start+size));
  }
  else {
    std::cerr << "Error: Don't know how to handle Knob Type (from): " << std::endl; assert(0);}
  //TODO: VECTORS!!!
  return new GNURadio::Knob();
}

pmt::pmt_t
rpcpmtconverter::to_pmt(const GNURadio::KnobPtr& knob, const Ice::Current& c)
{
  std::string id(knob->ice_id(c).substr(12));
  if(id == "KnobD") {
    GNURadio::KnobDPtr k(GNURadio::KnobDPtr::dynamicCast(knob));
    return pmt::mp(k->value);
  }
  else if(id == "KnobF") {
    GNURadio::KnobFPtr k(GNURadio::KnobFPtr::dynamicCast(knob));
    return pmt::mp(k->value);
  }
  else if(id == "KnobI") {
    GNURadio::KnobIPtr k(GNURadio::KnobIPtr::dynamicCast(knob));
    return pmt::mp(k->value);
  }
  else if(id == "KnobT") {
    GNURadio::KnobTPtr k(GNURadio::KnobTPtr::dynamicCast(knob));
    return pmt::mp(k->value);
  }
  else if(id == "KnobS") {
    GNURadio::KnobSPtr k(GNURadio::KnobSPtr::dynamicCast(knob));
    return pmt::string_to_symbol(k->value);
  }
  else if(id == "KnobB") {
    GNURadio::KnobBPtr k(GNURadio::KnobBPtr::dynamicCast(knob));
    return pmt::mp(k->value);
  }
  else if(id == "KnobC") {
    GNURadio::KnobCPtr k(GNURadio::KnobCPtr::dynamicCast(knob));
    return pmt::mp(k->value);
  }
  else if(id == "KnobL") {
    GNURadio::KnobLPtr k(GNURadio::KnobLPtr::dynamicCast(knob));
    return pmt::mp((long)k->value);
  } 
  else if(id == "KnobZ") {
    GNURadio::KnobZPtr k(GNURadio::KnobZPtr::dynamicCast(knob));
    std::complex<double> cpx(k->value.re, k->value.im);
    return pmt::from_complex(cpx);
  } 
  else if(id == "KnobVecC") {
    GNURadio::KnobVecCPtr k(GNURadio::KnobVecCPtr::dynamicCast(knob));
    return pmt::init_u8vector(k->value.size(), &k->value[0]);
  } 
  else if(id == "KnobVecI") {
    GNURadio::KnobVecIPtr k(GNURadio::KnobVecIPtr::dynamicCast(knob));
    return pmt::init_s32vector(k->value.size(), &k->value[0]);
  }
  //else if(id == "KnobVecF") {
  //  GNURadio::KnobVecFPtr k(GNURadio::KnobVecFPtr::dynamicCast(knob));
  //  return pmt::mp(k->value);
  //TODO: FLOAT!!!
  //TODO: VECTORS!!!
  else {
    std::cerr << "Error: Don't know how to handle Knob Type: " << id << std::endl; assert(0);
  }

  return pmt::pmt_t();	
}
