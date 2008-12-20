/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_PORT_SIMPLE_H
#define INCLUDED_MB_PORT_SIMPLE_H

#include <mblock/port.h>

/*!
 * \brief Concrete port realization
 */
class mb_port_simple : public mb_port
{
  bool			d_cache_valid;
  mb_msg_accepter_sptr	d_cached_accepter;

protected:
  static mb_msg_accepter_sptr
  find_accepter(mb_port_simple *start);

  mb_msg_accepter_sptr
  make_accepter();

public:
  mb_port_simple(mb_mblock *mblock,
		 const std::string &port_name,
		 const std::string &protocol_class_name,
		 bool conjugated,
		 mb_port::port_type_t port_type);

  ~mb_port_simple();

  /*!
   * \brief send a message
   *
   * \param signal	the event name
   * \param data	optional data
   * \param metadata	optional metadata
   * \param priority	the urgency at which the message is sent
   */
  void
  send(pmt_t signal,
       pmt_t data = PMT_NIL,
       pmt_t metadata = PMT_NIL,
       mb_pri_t priority = MB_PRI_DEFAULT);

  /*
   * \brief Invalidate any cached peer resolutions
   * \implementation
   */
  void invalidate_cache();

};

#endif /* INCLUDED_MB_PORT_SIMPLE_H */
