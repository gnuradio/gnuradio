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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_pdu.h>

size_t 
gr_pdu_itemsize(gr_pdu_vector_type type){
  switch(type){
  case pdu_byte:
    return 1;
  case pdu_float:
    return sizeof(float);
  case pdu_complex:
    return sizeof(gr_complex);
  default:
    throw std::runtime_error("bad type!");
  }
}

bool
gr_pdu_type_matches(gr_pdu_vector_type type, pmt::pmt_t v){
  switch(type){
  case pdu_byte:
    return pmt::pmt_is_u8vector(v);
  case pdu_float:
    return pmt::pmt_is_f32vector(v);
  case pdu_complex:
    return pmt::pmt_is_c32vector(v);
  default:
    throw std::runtime_error("bad type!");
  }
}

pmt::pmt_t
gr_pdu_make_vector(gr_pdu_vector_type type, const uint8_t* buf, size_t items){
  switch(type){
  case pdu_byte:
    return pmt::pmt_init_u8vector(items, buf);
  case pdu_float:
    return pmt::pmt_init_f32vector(items, (const float*)buf);
  case pdu_complex:
    return pmt::pmt_init_c32vector(items, (const gr_complex*)buf);
  default:
    throw std::runtime_error("bad type!");
  }
}

gr_pdu_vector_type type_from_pmt(pmt::pmt_t vector){
  if(pmt_is_u8vector(vector))
    return pdu_byte;
  if(pmt_is_f32vector(vector))
    return pdu_float;
  if(pmt_is_c32vector(vector))
    return pdu_complex;
  throw std::runtime_error("bad type!");
}
