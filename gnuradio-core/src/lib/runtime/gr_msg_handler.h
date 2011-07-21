/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef INCLUDED_GR_MSG_HANDLER_H
#define INCLUDED_GR_MSG_HANDLER_H

#include <gr_core_api.h>
#include <gr_message.h>

class gr_msg_handler;
typedef boost::shared_ptr<gr_msg_handler> gr_msg_handler_sptr;

/*!
 * \brief abstract class of message handlers
 * \ingroup base
 */
class GR_CORE_API gr_msg_handler {
public:
  virtual ~gr_msg_handler ();
  
  //! handle \p msg
  virtual void handle (gr_message_sptr msg) = 0;
};

#endif /* INCLUDED_GR_MSG_HANDLER_H */
