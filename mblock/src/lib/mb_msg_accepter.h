/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_MSG_ACCEPTER_H
#define INCLUDED_MB_MSG_ACCEPTER_H

#include <mb_common.h>

/*!
 * \brief Abstract class that accepts messages
 *
 * The mb_port::send method ultimately resolves the (local)
 * destination of a send to an object of this type.  The resulting 
 * object is called to deliver the message.
 *
 * Expect derived classes such as these:
 *
 *  smp      : target is visible in this address space
 *  mpi	     : target is on the other end of an MPI link
 *  ppe->spe : sending from Cell PPE to Cell SPE
 *  spe->ppe : sending from Cell SPE to Cell PPE
 */
class mb_msg_accepter {
public:
  mb_msg_accepter(){};
  virtual ~mb_msg_accepter();

  virtual void operator()(pmt_t signal, pmt_t data, pmt_t metadata, mb_pri_t priority) = 0;
};

#endif /* INCLUDED_MB_MSG_ACCEPTER_H */
