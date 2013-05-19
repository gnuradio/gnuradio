/* -*- c++ -*- */
/*
 * Copyright 2008,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PROBE_DENSITY_B_H
#define INCLUDED_GR_PROBE_DENSITY_B_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace digital {

    /*!
     * \ingroup measurement_tools_blk
     *
     * This block maintains a running average of the input stream and
     * makes it available as an accessor function. The input stream is
     * type unsigned char.
     *
     * If you send this block a stream of unpacked bytes, it will tell
     * you what the bit density is.
     */
    class DIGITAL_API probe_density_b : virtual public sync_block
    {
    public:
      // gr::digital::probe_density_b::sptr
      typedef boost::shared_ptr<probe_density_b> sptr;

      /*!
       * Make a density probe block.
       *
       * \param alpha  Average filter constant
       *
       */
      static sptr make(double alpha);

      /*!
       * \brief Returns the current density value
       */
      virtual double density() const = 0;

      /*!
       * \brief Set the average filter constant
       */
      virtual void set_alpha(double alpha) = 0;
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_PROBE_DENSITY_B_H */
