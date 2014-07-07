/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

// @WARNING@

#ifndef @GUARD_NAME@
#define @GUARD_NAME@

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief output = input * constant
     * \ingroup math_operators_blk
     */
    class BLOCKS_API @NAME@ : virtual public sync_block
    {

    public:

      // gr::blocks::@NAME@::sptr
      typedef boost::shared_ptr<@NAME@> sptr;

      /*!
       * \brief Create an instance of @NAME@
       * \param k multiplicative constant
       */
      static sptr make(@O_TYPE@ k);

      /*!
       * \brief Return multiplicative constant
       */
      virtual @O_TYPE@ k() const = 0;

      /*!
       * \brief Set multiplicative constant
       */
      virtual void set_k(@O_TYPE@ k) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* @GUARD_NAME */
