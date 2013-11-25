/* -*- c++ -*- */
/* 
 * Copyright 2013 Institute for Theoretical Information Technology,
 *                RWTH Aachen University
 * 
 * Authors: Johannes Schmitz <schmitz@ti.rwth-aachen.de>
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


#ifndef INCLUDED_ZMQBLOCKS_SOURCE_PUSHPULL_H
#define INCLUDED_ZMQBLOCKS_SOURCE_PUSHPULL_H

#include <zmqblocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace zmqblocks {

    /*!
     * \brief <+description of block+>
     * \ingroup zmqblocks
     *
     */
    class ZMQBLOCKS_API source_pushpull : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<source_pushpull> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of zmqblocks::source_pushpull.
       *
       * To avoid accidental use of raw pointers, zmqblocks::source_pushpull's
       * constructor is in a private implementation
       * class. zmqblocks::source_pushpull::make is the public interface for
       * creating new instances.
       */
      static sptr make(size_t itemsize, char *address);
    };

  } // namespace zmqblocks
} // namespace gr

#endif /* INCLUDED_ZMQBLOCKS_SOURCE_PUSHPULL_H */

