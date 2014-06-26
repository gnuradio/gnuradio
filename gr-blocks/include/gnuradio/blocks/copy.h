/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_COPY_H
#define INCLUDED_GR_COPY_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief output[i] = input[i]
     * \ingroup misc_blk
     *
     * \details
     * When enabled (default), this block copies its input to its
     * output. When disabled, this block drops its input on the floor.
     *
     * Message Ports:
     *
     * - en (input):
     *      Receives a PMT bool message to either enable to disable
     *      copy.
     */
    class BLOCKS_API copy : virtual public block
    {
    public:
      // gr::blocks::copy::sptr
      typedef boost::shared_ptr<copy> sptr;

      static sptr make(size_t itemsize);

      virtual void set_enabled(bool enable) = 0;
      virtual bool enabled() const = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_COPY_H */
