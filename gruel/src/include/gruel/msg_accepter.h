/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MSG_ACCEPTER_H
#define INCLUDED_MSG_ACCEPTER_H

#include <gruel/pmt.h>

namespace gruel {

  /*!
   * \brief Virtual base class that accepts messages
   */
  class msg_accepter
  {
  public:
    msg_accepter() {};
    virtual ~msg_accepter();

    virtual void post(pmt::pmt_t msg) = 0;
  };

} /* namespace gruel */

#endif /* INCLUDED_MSG_ACCEPTER_H */
