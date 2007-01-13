/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#include <mb_protocol_class.h>

static pmt_t s_ALL_PROTOCOL_CLASSES = PMT_NIL;

pmt_t 
mb_make_protocol_class(pmt_t name, pmt_t incoming, pmt_t outgoing)
{
  // (protocol-class <name> <incoming> <outgoing>)

  if (!pmt_is_symbol(name))
    throw pmt_wrong_type("mb_make_protocol_class: NAME must be symbol", name);
  if (!(pmt_is_pair(incoming) || pmt_is_null(incoming)))
    throw pmt_wrong_type("mb_make_protocol_class: INCOMING must be a list", name);
  if (!(pmt_is_pair(outgoing) || pmt_is_null(outgoing)))
    throw pmt_wrong_type("mb_make_protocol_class: OUTGOING must be a list", name);

  pmt_t t = pmt_cons(pmt_intern("protocol-class"),
		     pmt_cons(name,
			      pmt_cons(incoming,
				       pmt_cons(outgoing, PMT_NIL))));

  // Remember this protocol class.
  s_ALL_PROTOCOL_CLASSES = pmt_cons(t, s_ALL_PROTOCOL_CLASSES);
  return t;
}

pmt_t
mb_protocol_class_name(pmt_t pc)
{
  return pmt_nth(1, pc);
}

pmt_t
mb_protocol_class_incoming(pmt_t pc)
{
  return pmt_nth(2, pc);
}

pmt_t
mb_protocol_class_outgoing(pmt_t pc)
{
  return pmt_nth(3, pc);
}

pmt_t
mb_protocol_class_lookup(pmt_t name)
{
  pmt_t lst = s_ALL_PROTOCOL_CLASSES;

  while (pmt_is_pair(lst)){
    if (pmt_eq(name, mb_protocol_class_name(pmt_car(lst))))
      return pmt_car(lst);
    lst = pmt_cdr(lst);
  }

  return PMT_NIL;
}
