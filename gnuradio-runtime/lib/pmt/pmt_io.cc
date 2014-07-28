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
#include <pmt/pmt.h>
#include "pmt_int.h"
#include <sstream>
#include <iostream>

namespace pmt {

static void
write_list_tail(pmt_t obj, std::ostream &port)
{
  write(car(obj), port); // write the car
  obj = cdr(obj);		 // step to cdr

  if (is_null(obj))		 // ()
    port << ")";

  else if (is_pair(obj)){	 // normal list
    port << " ";
    write_list_tail(obj, port);
  }
  else {			 // dotted pair
    port << " . ";
    write(obj, port);
    port << ")";
  }
}

void
write(pmt_t obj, std::ostream &port)
{
  if (is_bool(obj)){
    if (is_true(obj))
      port << "#t";
    else
      port << "#f";
  }
  else if (is_symbol(obj)){
    port << symbol_to_string(obj);
  }
  else if (is_number(obj)){
    if (is_integer(obj))
      port << to_long(obj);
    else if (is_uint64(obj))
      port << to_uint64(obj);
    else if (is_real(obj))
      port << to_double(obj);
    else if (is_complex(obj)){
      std::complex<double> c = to_complex(obj);
      port << c.real() << '+' << c.imag() << 'i';
    }
    else
      goto error;
  }
  else if (is_null(obj)){
    port << "()";
  }
  else if (is_pair(obj)){
    port << "(";
    write_list_tail(obj, port);
  }
  else if (is_tuple(obj)){
    port << "{";
    size_t len = length(obj);
    if (len > 0){
      port << tuple_ref(obj, 0);
      for (size_t i = 1; i < len; i++)
	port << " " << tuple_ref(obj, i);
    }
    port << "}";
  }
  else if (is_vector(obj)){
    port << "#(";
    size_t len = length(obj);
    if (len > 0){
      port << vector_ref(obj, 0);
      for (size_t i = 1; i < len; i++)
	port << " " << vector_ref(obj, i);
    }
    port << ")";
  }
  else if (is_dict(obj)){
    // FIXME
    // port << "#<dict " << obj << ">";
    port << "#<dict>";
  }
  else if (is_uniform_vector(obj)){
    port << "#[";
    size_t len = length(obj);
    if (len)
    {
      pmt_uniform_vector *uv = static_cast<pmt_uniform_vector*>(obj.get());
      port << uv->string_ref(0);
      for (size_t i = 1; i < len; i++)
	port << " " << uv->string_ref(i);
    }
    port <<  "]";
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
  write(obj, os);
  return os;
}

std::string
write_string(pmt_t obj)
{
  std::ostringstream s;
  s << obj;
  return s.str();
}

pmt_t
read(std::istream &port)
{
  throw notimplemented("notimplemented: pmt::read", PMT_NIL);
}

void
serialize(pmt_t obj, std::ostream &sink)
{
  throw notimplemented("notimplemented: pmt::serialize", obj);
}

/*!
 * \brief Create obj from portable byte-serial representation
 */
pmt_t
deserialize(std::istream &source)
{
  throw notimplemented("notimplemented: pmt::deserialize", PMT_NIL);
}

} /* namespace pmt */


void
pmt::print(pmt_t v)
{
  std::cout << write_string(v) << std::endl;
}


