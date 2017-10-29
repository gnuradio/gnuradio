/* -*- c++ -*- */
/*
 * Copyright 2015-2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DTV_DVB_BBHEADER_RAW_BB_H
#define INCLUDED_DTV_DVB_BBHEADER_RAW_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt2_config.h>
#include <gnuradio/dtv/dvbs2_config.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Formats raw packets into Single Generic Stream FEC baseband frames
     * and adds a 10-byte header.
     * \ingroup dtv
     *
     * \details
     * Input: 188-byte Raw Stream packets. \n
     * Output: Variable length FEC baseband frames (BBFRAME). The output frame
     *         length is based on the FEC rate.
     */
    class DTV_API dvb_bbheader_raw_bb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<dvb_bbheader_raw_bb> sptr;

      /*!
       * \brief Create a baseband header formatter.
       *
       * \param standard DVB standard (DVB-S2 or DVB-T2).
       * \param framesize FEC frame size (normal, medium or short).
       * \param rate FEC code rate.
       * \param rolloff DVB-S2 root-raised-cosine filter roll-off.
       * \param mode DVB-T2 input processing mode.
       * \param inband DVB-T2 Type B in-band signalling.
       * \param fecblocks DVB-T2 number of FEC block for in-band signalling.
       * \param tsrate DVB-T2 Transport Stream rate for in-band signalling.
       * \param transportType Transport Type TS_GS_TRANSPORT = 3, TS_GS_GENERIC_PACKETIZED = 0 TS_GS_GENERIC_CONTINUOUS = 1 TS_GS_RESERVED = 2
       * \param singleStream DVB-S2 contains single stream or multiple streams
       * \param ccm DVB-S2 stream is CCM or ACM
       * \param issyi DVB-S2 ISSYI is enabled
       * \param npd DVB-S2 NPD is active
       * \param upl DVB-S2 User Packet Length
       */
      static sptr make(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate, dvbs2_rolloff_factor_t rolloff, dvbt2_inputmode_t mode, dvbt2_inband_t inband, int fecblocks, int tsrate, int transportType, bool singleStream, bool ccm, bool issyi, bool npd, int upl);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_RAW_BBHEADER_BB_H */

