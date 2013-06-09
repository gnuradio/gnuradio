/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_NOP_H
#define INCLUDED_GR_NOP_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>
#include <stddef.h>			// size_t

namespace gr {
  namespace blocks {

    /*!
     * \brief Does nothing. Used for testing only.
     * \ingroup misc_blk
     */
    class BLOCKS_API nop : virtual public block
    {
    public:
      // gr::blocks::nop::sptr
      typedef boost::shared_ptr<nop> sptr;

      /*!
       * Build a nop block.
       *
       * \param sizeof_stream_item size of the stream items in bytes.
       */
      static sptr make(size_t sizeof_stream_item);

      virtual int nmsgs_received() const = 0;

      virtual int  ctrlport_test() const = 0;
      virtual void set_ctrlport_test(int x) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_NOP_H */
