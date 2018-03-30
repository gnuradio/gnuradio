/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/blocks/pdu.h>

namespace gr {
  namespace blocks {
    namespace pdu {

      const
      pmt::pmt_t pdu_port_id(){
        static const pmt::pmt_t pdu_port_id = pmt::mp("pdus");
        return pdu_port_id;
      }

      size_t
      itemsize(vector_type type)
      {
	switch(type) {
	case byte_t:
	  return sizeof(char);
	case float_t:
	  return sizeof(float);
	case complex_t:
	  return sizeof(gr_complex);
	default:
	  throw std::runtime_error("bad PDU type");
	}
      }

      bool
      type_matches(vector_type type, pmt::pmt_t v)
      {
	switch(type) {
	case byte_t:
	  return pmt::is_u8vector(v);
	case float_t:
	  return pmt::is_f32vector(v);
	case complex_t:
	  return pmt::is_c32vector(v);
	default:
	  throw std::runtime_error("bad PDU type");
	}
      }

      pmt::pmt_t
      make_pdu_vector(vector_type type, const uint8_t *buf, size_t items)
      {
	switch(type) {
	case byte_t:
	  return pmt::init_u8vector(items, buf);
	case float_t:
	  return pmt::init_f32vector(items, (const float *)buf);
	case complex_t:
	  return pmt::init_c32vector(items, (const gr_complex *)buf);
	default:
	  throw std::runtime_error("bad PDU type");
	}
      }

      vector_type
      type_from_pmt(pmt::pmt_t vector)
      {
	if(pmt::is_u8vector(vector))
	  return byte_t;
	if(pmt::is_f32vector(vector))
	  return float_t;
	if(pmt::is_c32vector(vector))
	  return complex_t;
	throw std::runtime_error("bad PDU type");
      }

    } /* namespace pdu */
  } /* namespace blocks */
} /* namespace gr */
