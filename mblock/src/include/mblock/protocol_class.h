/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_PROTOCOL_CLASS_H
#define INCLUDED_MB_PROTOCOL_CLASS_H

#include <mblock/common.h>

/*!
 * \brief construct a protocol_class
 *
 * \param name		the name of the class (symbol)
 * \param incoming	incoming message set (list of symbols)
 * \param outgoing	outgoing message set (list of symbols)
 */
gruel::pmt_t mb_make_protocol_class(gruel::pmt_t name, gruel::pmt_t incoming, gruel::pmt_t outgoing);

// Accessors
gruel::pmt_t mb_protocol_class_name(gruel::pmt_t pc);		//< return name of protocol class
gruel::pmt_t mb_protocol_class_incoming(gruel::pmt_t pc);	//< return incoming message set
gruel::pmt_t mb_protocol_class_outgoing(gruel::pmt_t pc);	//< return outgoing message set

gruel::pmt_t mb_protocol_class_lookup(gruel::pmt_t name);	//< lookup an existing protocol class by name


/*!
 * \brief Initialize one or more protocol class from a serialized description.
 * Used by machine generated code.
 */
class mb_protocol_class_init {
public:
  mb_protocol_class_init(const char *data, size_t len);
};

#endif /* INCLUDED_MB_PROTOCOL_CLASS_H */
