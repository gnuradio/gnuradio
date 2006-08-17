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
#ifndef INCLUDED_MB_PROTOCOL_CLASS_H
#define INCLUDED_MB_PROTOCOL_CLASS_H

#include <mb_common.h>

// FIXME maybe this should just be a pmt_t aggregate???

class mb_protocol_class;
typedef boost::shared_ptr<mb_protocol_class> mb_protocol_class_sptr;

/*!
 * \brief construct a protocol_class and return boost::shared_ptr
 *
 * \param name		the name of the class (symbol)
 * \param incoming	incoming message set (dict: keys are message types)
 * \param outgoing	outgoing message set (dict: keys are message types)
 */
mb_protocol_class_sptr 
mb_make_protocol_class(pmt_t name, pmt_t incoming, pmt_t outgoing);

class mb_protocol_class {
  pmt_t		d_name;
  pmt_t		d_incoming;
  pmt_t		d_outgoing;

  friend mb_protocol_class_sptr
  mb_make_protocol_class(pmt_t name, pmt_t incoming, pmt_t outgoing);

  // private constructor
  mb_protocol_class(pmt_t name, pmt_t incoming, pmt_t outgoing);

public:
  ~mb_protocol_class();

  pmt_t name() const { return d_name; }
  pmt_t incoming() const { return d_incoming; }
  pmt_t outgoing() const { return d_outgoing; }

  mb_protocol_class_sptr conj() const;	// return the conjugate of this protocol class
};

#endif /* INCLUDED_MB_PROTOCOL_CLASS_H */
