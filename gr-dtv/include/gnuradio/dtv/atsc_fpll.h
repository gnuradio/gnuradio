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

#ifndef INCLUDED_DTV_ATSC_FPLL_H
#define INCLUDED_DTV_ATSC_FPLL_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief ATSC Receiver FPLL
     *
     * This block is takes in a complex I/Q baseband stream from the
     * receive filter and outputs the 8-level symbol stream.
     *
     * It does this by first locally generating a pilot tone and
     * complex mixing with the input signal.  This results in the
     * pilot tone shifting to DC and places the signal in the upper
     * sideband.
     *
     * As no information is encoded in the phase of the waveform, the
     * Q channel is then discarded, producing a real signal with the
     * lower sideband restored.
     *
     * The 8-level symbol stream still has a DC offset, and still
     * requires symbol timing recovery.
     *
     * \ingroup dtv_atsc
     */
    class DTV_API atsc_fpll : virtual public gr::sync_block
    {
    public:

      // gr::dtv::atsc_fpll::sptr
      typedef boost::shared_ptr<atsc_fpll> sptr;

      /*!
       * \brief Make a new instance of gr::dtv::atsc_fpll.
       *
       * param rate  Sample rate of incoming stream
       */
      static sptr make(float rate);
    };

  } /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_FPLL_H */
