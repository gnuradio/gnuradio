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

/*!
 * \brief construct a protocol_class
 *
 * \param name		the name of the class (symbol)
 * \param incoming	incoming message set (list of symbols)
 * \param outgoing	outgoing message set (list of symbols)
 */
pmt_t mb_make_protocol_class(pmt_t name, pmt_t incoming, pmt_t outgoing);

// Accessors
pmt_t mb_protocol_class_name(pmt_t pc);		//< return name of protocol class
pmt_t mb_protocol_class_incoming(pmt_t pc);	//< return incoming message set
pmt_t mb_protocol_class_outgoing(pmt_t pc);	//< return outgoing message set

pmt_t mb_protocol_class_lookup(pmt_t name);	//< lookup an existing protocol class by name


/*!
 * \brief Initialize one or more protocol class from a serialized description.
 * Used by machine generated code.
 */
class mb_protocol_class_init {
public:
  mb_protocol_class_init(const char *data, size_t len);
};

#endif /* INCLUDED_MB_PROTOCOL_CLASS_H */
