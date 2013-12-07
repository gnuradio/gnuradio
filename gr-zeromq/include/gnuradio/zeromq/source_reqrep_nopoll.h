/* -*- c++ -*- */
/* 
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_ZEROMQ_SOURCE_REQREP_NOPOLL_H
#define INCLUDED_ZEROMQ_SOURCE_REQREP_NOPOLL_H

#include <gnuradio/zeromq/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace zeromq {

    /*!
     * \brief <+description of block+>
     * \ingroup zeromq
     *
     */
    class ZEROMQ_API source_reqrep_nopoll : virtual public gr::sync_block
    {
    public:
      typedef boost::shared_ptr<source_reqrep_nopoll> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of zeromq::source_reqrep_nopoll.
       *
       * To avoid accidental use of raw pointers, zeromq::source_reqrep_nopoll's
       * constructor is in a private implementation
       * class. zeromq::source_reqrep_nopoll::make is the public interface for
       * creating new instances.
       */
      static sptr make(size_t itemsize, char *address);
    };

  } // namespace zeromq
} // namespace gr

#endif /* INCLUDED_ZEROMQ_SOURCE_REQREP_NOPOLL_H */
