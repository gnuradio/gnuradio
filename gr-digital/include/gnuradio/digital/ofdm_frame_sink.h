/* -*- c++ -*- */
/*
 * Copyright 2007,2011,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_FRAME_SINK_H
#define INCLUDED_DIGITAL_OFDM_FRAME_SINK_H

#include <gnuradio/digital/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace digital {
    
    /*!
     * \brief Takes an OFDM symbol in, demaps it into bits of 0's and
     * 1's, packs them into packets, and sends to to a message queue
     * sink.
     * \ingroup ofdm_blk
     * \ingroup deprecated_blk
     *
     * \details
     * NOTE: The mod input parameter simply chooses a pre-defined
     * demapper/slicer. Eventually, we want to be able to pass in a
     * reference to an object to do the demapping and slicing for a
     * given modulation type.
     */
    class DIGITAL_API ofdm_frame_sink : virtual public sync_block
    {
    public:
      // gr::digital::ofdm_frame_sink::sptr
      typedef boost::shared_ptr<ofdm_frame_sink> sptr;

      /*!
       * Make an OFDM frame sink block.
       *
       * \param sym_position vector of OFDM carrier symbols in complex space
       * \param sym_value_out vector of bit mapped from the complex symbol space
       * \param target_queue message queue for the packets to go into
       * \param occupied_tones The number of subcarriers with data in the received symbol
       * \param phase_gain gain of the phase tracking loop
       * \param freq_gain gain of the frequency tracking loop
       */
      static sptr make(const std::vector<gr_complex> &sym_position, 
		       const std::vector<char> &sym_value_out,
		       msg_queue::sptr target_queue,
		       int occupied_tones,
		       float phase_gain=0.25, float freq_gain=0.25*0.25/4);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_OFDM_FRAME_SINK_H */
