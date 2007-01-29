/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#include <mb_port_simple.h>
#include <mb_msg_accepter.h>

mb_port_simple::mb_port_simple(mb_mblock *mblock,
			       const std::string &port_name,
			       const std::string &protocol_class_name,
			       bool conjugated,
			       mb_port::port_type_t port_type)
  : mb_port(mblock, port_name, protocol_class_name, conjugated, port_type)
{
}

mb_port_simple::~mb_port_simple()
{
  // nop
}

void
mb_port_simple::send(pmt_t signal, pmt_t data, pmt_t metadata, mb_pri_t priority)
{
  mb_msg_accepter_sptr  accepter = find_accepter();
  if (accepter)
    (*accepter)(signal, data, metadata, priority);
}

mb_msg_accepter_sptr
mb_port_simple::find_accepter()
{
  mb_msg_accepter_sptr	r;

  // FIXME, actually do the work ;)

  return r;
}
