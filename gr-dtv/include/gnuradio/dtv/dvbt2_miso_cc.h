/* -*- c++ -*- */
/* 
 * Copyright 2015,2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DTV_DVBT2_MISO_CC_H
#define INCLUDED_DTV_DVBT2_MISO_CC_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt2_config.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Splits the stream for MISO (Multiple Input Single Output).
     * \ingroup dtv
     *
     * Input: Frequency interleaved T2 frame. \n
     * Output1: Frequency interleaved T2 frame (copy of input). \n
     * Output2: Frequency interleaved T2 frame with modified Alamouti processing.
     */
    class DTV_API dvbt2_miso_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<dvbt2_miso_cc> sptr;

      /*!
       * \brief Create a MISO processor.
       *
       * \param carriermode number of carriers (normal or extended).
       * \param fftsize OFDM IFFT size.
       * \param pilotpattern DVB-T2 pilot pattern (PP1 - PP8).
       * \param guardinterval OFDM ISI guard interval.
       * \param numdatasyms number of OFDM symbols in a T2 frame.
       * \param paprmode PAPR reduction mode.
       */
      static sptr make(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvbt2_pilotpattern_t pilotpattern, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_papr_t paprmode);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_MISO_CC_H */

