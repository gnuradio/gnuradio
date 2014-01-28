/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_ROTATOR_CC_H
#define INCLUDED_BLOCKS_ROTATOR_CC_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Complex rotator
     * \ingroup math_operators_blk
     */
    class BLOCKS_API rotator_cc : virtual public sync_block
    {
    public:
      // gr::blocks::rotator_cc::sptr
      typedef boost::shared_ptr<rotator_cc> sptr;

      /*!
       * \brief Make an complex rotator block
       * \param phase_inc rotational velocity
       */
      static sptr make(double phase_inc = 0.0);

      virtual void set_phase_inc(double phase_inc) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_ROTATOR_CC_H */
