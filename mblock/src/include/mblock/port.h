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
#ifndef INCLUDED_MB_PORT_H
#define INCLUDED_MB_PORT_H

#include <mblock/common.h>

/*!
 * \brief Abstract port characteristics
 */
class mb_port : boost::noncopyable
{
public:

  //! port classification
  enum port_type_t {
    EXTERNAL,	//< Externally visible
    RELAY,	//< Externally visible but really connected to a sub-component
    INTERNAL	//< Visible to self only
  };

private:

  std::string		d_port_name;
  gruel::pmt_t		d_port_symbol;		// the port_name as a gruel::pmt symbol
  gruel::pmt_t		d_protocol_class;
  bool			d_conjugated;
  port_type_t		d_port_type;

protected:
  mb_mblock	       *d_mblock;  // mblock we're defined in

  // protected constructor
  mb_port(mb_mblock *mblock,
	  const std::string &port_name,
	  const std::string &protocol_class_name,
	  bool conjugated,
	  mb_port::port_type_t port_type);

  mb_mblock *mblock() const { return d_mblock; }

public:
  std::string	port_name() const { return d_port_name; }
  gruel::pmt_t	port_symbol() const { return d_port_symbol; }
  gruel::pmt_t	protocol_class() const { return d_protocol_class; }
  bool          conjugated() const { return d_conjugated; }
  port_type_t	port_type() const { return d_port_type; }

  gruel::pmt_t		incoming_message_set() const;
  gruel::pmt_t		outgoing_message_set() const;

  virtual ~mb_port();

  /*!
   * \brief send a message
   *
   * \param signal	the event name
   * \param data	optional data
   * \param metadata	optional metadata
   * \param priority	the urgency at which the message is sent
   */
  virtual void
  send(gruel::pmt_t signal,
       gruel::pmt_t data = gruel::PMT_F,
       gruel::pmt_t metadata = gruel::PMT_F,
       mb_pri_t priority = MB_PRI_DEFAULT) = 0;

  /*
   * \brief Invalidate any cached peer resolutions
   * \internal
   */
  virtual void invalidate_cache() = 0;
};

#endif /* INCLUDED_MB_PORT_H */
