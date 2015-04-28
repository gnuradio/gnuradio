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

#ifndef RPCPMTCONVERTERS_THRIFT_H
#define RPCPMTCONVERTERS_THRIFT_H

#include <pmt/pmt.h>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include "thrift/gnuradio_types.h"


namespace rpcpmtconverter
{
  GNURadio::Knob from_pmt(const pmt::pmt_t& knob);

  struct to_pmt_f {
    to_pmt_f() {;}
    virtual ~to_pmt_f() {}
    virtual pmt::pmt_t operator()(const GNURadio::Knob& knob);
  };

  struct to_pmt_byte_f   : public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_short_f  : public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_int_f    : public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_long_f   : public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_double_f : public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_string_f : public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_bool_f   : public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_complex_f: public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_f32vect_f: public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_f64vect_f: public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_s64vect_f: public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_s32vect_f: public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_s16vect_f: public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_s8vect_f : public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };
  struct to_pmt_c32vect_f: public to_pmt_f { pmt::pmt_t operator()(const GNURadio::Knob& knob); };

  class To_PMT : private boost::noncopyable {
   public:
     static To_PMT instance;
     template<typename TO_PMT_F> friend struct to_pmt_reg;
     pmt::pmt_t operator()(const GNURadio::Knob& knob);

   protected:
     boost::ptr_map<GNURadio::BaseTypes::type, to_pmt_f> to_pmt_map;

  private:
    To_PMT() {;}
  };

  template<typename TO_PMT_F> struct to_pmt_reg {
    to_pmt_reg(To_PMT& instance, const GNURadio::BaseTypes::type type);
  };
}

#endif /* RPCPMTCONVERTERS_THRIFT_H */
