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


mb_protocol_class_sptr 
mb_make_protocol_class(pmt_t name, pmt_t incoming, pmt_t outgoing)
{
  return mb_protocol_class_sptr(new mb_protocol_class(name, incoming, outgoing));
}

mb_protocol_class::mb_protocol_class(pmt_t name, pmt_t incoming, pmt_t outgoing)
  : d_name(name), d_incoming(incoming), d_outgoing(outgoing)
{
}

mb_protocol_class::~mb_protocol_class()
{
  // NOP
}


mb_protocol_class_sptr
mb_protocol_class::conj() const
{
  return mb_make_protocol_class(name(), outgoing(), incoming());
}
