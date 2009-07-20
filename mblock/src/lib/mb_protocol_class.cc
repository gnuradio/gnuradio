/* -*- c++ -*- */
/*
 * Copyright 2006,2008 Free Software Foundation, Inc.
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

#include <mblock/protocol_class.h>
#include <iostream>

static gruel::pmt_t s_ALL_PROTOCOL_CLASSES = gruel::PMT_NIL;

gruel::pmt_t 
mb_make_protocol_class(gruel::pmt_t name, gruel::pmt_t incoming, gruel::pmt_t outgoing)
{
  // (protocol-class <name> <incoming> <outgoing>)

  if (!gruel::pmt_is_symbol(name))
    throw gruel::pmt_wrong_type("mb_make_protocol_class: NAME must be symbol", name);
  if (!(gruel::pmt_is_pair(incoming) || gruel::pmt_is_null(incoming)))
    throw gruel::pmt_wrong_type("mb_make_protocol_class: INCOMING must be a list", name);
  if (!(gruel::pmt_is_pair(outgoing) || gruel::pmt_is_null(outgoing)))
    throw gruel::pmt_wrong_type("mb_make_protocol_class: OUTGOING must be a list", name);

  gruel::pmt_t t = gruel::pmt_cons(gruel::pmt_intern("protocol-class"),
		     gruel::pmt_cons(name,
			      gruel::pmt_cons(incoming,
				       gruel::pmt_cons(outgoing, gruel::PMT_NIL))));

  // Remember this protocol class.
  s_ALL_PROTOCOL_CLASSES = gruel::pmt_cons(t, s_ALL_PROTOCOL_CLASSES);
  return t;
}

gruel::pmt_t
mb_protocol_class_name(gruel::pmt_t pc)
{
  return gruel::pmt_nth(1, pc);
}

gruel::pmt_t
mb_protocol_class_incoming(gruel::pmt_t pc)
{
  return gruel::pmt_nth(2, pc);
}

gruel::pmt_t
mb_protocol_class_outgoing(gruel::pmt_t pc)
{
  return gruel::pmt_nth(3, pc);
}

gruel::pmt_t
mb_protocol_class_lookup(gruel::pmt_t name)
{
  gruel::pmt_t lst = s_ALL_PROTOCOL_CLASSES;

  while (gruel::pmt_is_pair(lst)){
    if (gruel::pmt_eq(name, mb_protocol_class_name(gruel::pmt_car(lst))))
      return gruel::pmt_car(lst);
    lst = gruel::pmt_cdr(lst);
  }

  return gruel::PMT_NIL;
}

mb_protocol_class_init::mb_protocol_class_init(const char *data, size_t len)
{
  std::stringbuf sb;
  sb.str(std::string(data, len));

  while (1){
    gruel::pmt_t obj = gruel::pmt_deserialize(sb);

    if (0){
      gruel::pmt_write(obj, std::cout);
      std::cout << std::endl;
    }

    if (gruel::pmt_is_eof_object(obj))
      return;

    mb_make_protocol_class(gruel::pmt_nth(0, obj),   // protocol-class name
			   gruel::pmt_nth(1, obj),   // list of incoming msg names
			   gruel::pmt_nth(2, obj));  // list of outgoing msg names
  }
}
