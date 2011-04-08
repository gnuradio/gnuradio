/* -*- c++ -*- */
/*
 * Copyright 2006,2009 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vector>
#include <gruel/pmt.h>
#include "pmt_int.h"
#include <sstream>
#include <iostream>

namespace pmt {

static void
pmt_write_list_tail(pmt_t obj, std::ostream &port)
{
  pmt_write(pmt_car(obj), port); // write the car
  obj = pmt_cdr(obj);		 // step to cdr

  if (pmt_is_null(obj))		 // ()
    port << ")";

  else if (pmt_is_pair(obj)){	 // normal list
    port << " ";
    pmt_write_list_tail(obj, port);
  }
  else {			 // dotted pair
    port << " . ";
    pmt_write(obj, port);
    port << ")";
  }
}

void
pmt_write(pmt_t obj, std::ostream &port)
{
  if (pmt_is_bool(obj)){
    if (pmt_is_true(obj))
      port << "#t";
    else
      port << "#f";
  }
  else if (pmt_is_symbol(obj)){
    port << pmt_symbol_to_string(obj);
  }
  else if (pmt_is_number(obj)){
    if (pmt_is_integer(obj))
      port << pmt_to_long(obj);
    else if (pmt_is_uint64(obj))
      port << pmt_to_uint64(obj);
    else if (pmt_is_real(obj))
      port << pmt_to_double(obj);
    else if (pmt_is_complex(obj)){
      std::complex<double> c = pmt_to_complex(obj);
      port << c.real() << '+' << c.imag() << 'i';
    }
    else
      goto error;
  }
  else if (pmt_is_null(obj)){
    port << "()";
  }
  else if (pmt_is_pair(obj)){
    port << "(";
    pmt_write_list_tail(obj, port);
  }
  else if (pmt_is_tuple(obj)){
    port << "{";
    size_t len = pmt_length(obj);
    if (len > 0){
      port << pmt_tuple_ref(obj, 0);
      for (size_t i = 1; i < len; i++)
	port << " " << pmt_tuple_ref(obj, i);
    }
    port << "}";
  }
  else if (pmt_is_vector(obj)){
    port << "#(";
    size_t len = pmt_length(obj);
    if (len > 0){
      port << pmt_vector_ref(obj, 0);
      for (size_t i = 1; i < len; i++)
	port << " " << pmt_vector_ref(obj, i);
    }
    port << ")";
  }
  else if (pmt_is_dict(obj)){
    // FIXME
    // port << "#<dict " << obj << ">";
    port << "#<dict>";
  }
  else if (pmt_is_uniform_vector(obj)){
    // FIXME
    // port << "#<uniform-vector " << obj << ">";
    port << "#<uniform-vector>";
  }
  else {
  error:
    // FIXME
    // port << "#<" << obj << ">";
    port << "#<unknown>";
  }
}

std::ostream& operator<<(std::ostream &os, pmt_t obj)
{
  pmt_write(obj, os);
  return os;
}

std::string 
pmt_write_string(pmt_t obj)
{
  std::ostringstream s;
  s << obj;
  return s.str();
}

pmt_t
pmt_read(std::istream &port)
{
  throw pmt_notimplemented("notimplemented: pmt_read", PMT_NIL);
}

void
pmt_serialize(pmt_t obj, std::ostream &sink)
{
  throw pmt_notimplemented("notimplemented: pmt_serialize", obj);
}

/*!
 * \brief Create obj from portable byte-serial representation
 */
pmt_t 
pmt_deserialize(std::istream &source)
{
  throw pmt_notimplemented("notimplemented: pmt_deserialize", PMT_NIL);
}

} /* namespace pmt */


void 
pmt::pmt_print(pmt_t v)
{
  std::cout << pmt_write_string(v) << std::endl;
}


