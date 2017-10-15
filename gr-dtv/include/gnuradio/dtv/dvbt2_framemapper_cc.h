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

#ifndef INCLUDED_DTV_DVBT2_FRAMEMAPPER_CC_H
#define INCLUDED_DTV_DVBT2_FRAMEMAPPER_CC_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt2_config.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Maps T2 frames.
     * \ingroup dtv
     *
     * Input: Cell and time interleaved QPSK, 16QAM, 64QAM or 256QAM modulated cells. \n
     * Output: T2 frame.
     */
    class DTV_API dvbt2_framemapper_cc : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<dvbt2_framemapper_cc> sptr;

      /*!
       * \brief Create a DVB-T2 frame mapper.
       *
       * \param framesize FEC frame size (normal or short).
       * \param rate FEC code rate.
       * \param constellation DVB-T2 constellation.
       * \param rotation DVB-T2 constellation rotation (on or off).
       * \param fecblocks number of FEC frames in a T2 frame.
       * \param tiblocks number of time interleaving blocks in a T2 frame.
       * \param carriermode number of carriers (normal or extended).
       * \param fftsize OFDM IFFT size.
       * \param guardinterval OFDM ISI guard interval.
       * \param l1constellation L1 constellation.
       * \param pilotpattern DVB-T2 pilot pattern (PP1 - PP8).
       * \param t2frames number of T2 frames in a super-frame.
       * \param numdatasyms number of OFDM symbols in a T2 frame.
       * \param paprmode PAPR reduction mode.
       * \param version DVB-T2 specification version.
       * \param preamble P1 symbol preamble format.
       * \param inputmode Baseband Header mode.
       * \param reservedbiasbits set all L1 bias bits to 1 (on or off).
       * \param l1scrambled scramble L1 post signalling (on or off).
       * \param inband In-band type B signalling (on or off).
       */
      static sptr make(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation, dvbt2_rotation_t rotation, int fecblocks, int tiblocks, dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvb_guardinterval_t guardinterval, dvbt2_l1constellation_t l1constellation, dvbt2_pilotpattern_t pilotpattern, int t2frames, int numdatasyms, dvbt2_papr_t paprmode, dvbt2_version_t version, dvbt2_preamble_t preamble, dvbt2_inputmode_t inputmode, dvbt2_reservedbiasbits_t reservedbiasbits, dvbt2_l1scrambled_t l1scrambled, dvbt2_inband_t inband);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_FRAMEMAPPER_CC_H */

