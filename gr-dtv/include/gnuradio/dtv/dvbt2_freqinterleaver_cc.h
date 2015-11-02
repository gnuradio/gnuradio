/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DTV_DVBT2_FREQINTERLEAVER_CC_H
#define INCLUDED_DTV_DVBT2_FREQINTERLEAVER_CC_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt2_config.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Frequency interleaves a T2 frame.
     * \ingroup dtv
     *
     * Input: T2 frame.
     * Output: Frequency interleaved T2 frame.
     */
    class DTV_API dvbt2_freqinterleaver_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<dvbt2_freqinterleaver_cc> sptr;

      /*!
       * \brief Create a DVB-T2 frequency interleaver.
       *
       * \param carriermode number of carriers (normal or extended).
       * \param fftsize OFDM IFFT size.
       * \param pilotpattern DVB-T2 pilot pattern (PP1 - PP8).
       * \param guardinterval OFDM ISI guard interval.
       * \param numdatasyms number of OFDM symbols in a T2 frame.
       * \param paprmode PAPR reduction mode.
       * \param version DVB-T2 specification version.
       * \param preamble P1 symbol preamble format.
       */
      static sptr make(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode, dvbt2_version_t version, dvbt2_preamble_t preamble);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_FREQINTERLEAVER_CC_H */

