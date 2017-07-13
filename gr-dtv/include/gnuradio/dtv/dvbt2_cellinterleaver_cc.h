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

#ifndef INCLUDED_DTV_DVBT2_CELLINTERLEAVER_CC_H
#define INCLUDED_DTV_DVBT2_CELLINTERLEAVER_CC_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Cell and time interleaves QPSK/QAM modulated cells.
     * \ingroup dtv
     *
     * Input: QPSK, 16QAM, 64QAM or 256QAM modulated cells. \n
     * Output: Cell and time interleaved QPSK, 16QAM, 64QAM or 256QAM modulated cells.
     */
    class DTV_API dvbt2_cellinterleaver_cc : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<dvbt2_cellinterleaver_cc> sptr;

      /*!
       * \brief Create a DVB-T2 cell and time interleaver.
       *
       * \param framesize FEC frame size (normal or short).
       * \param constellation DVB-T2 constellation.
       * \param fecblocks number of FEC frames in a T2 frame.
       * \param tiblocks number of time interleaving blocks in a T2 frame.
       */
      static sptr make(dvb_framesize_t framesize, dvb_constellation_t constellation, int fecblocks, int tiblocks);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_CELLINTERLEAVER_CC_H */

