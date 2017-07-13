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

#ifndef INCLUDED_DTV_DVBS2_INTERLEAVER_BB_H
#define INCLUDED_DTV_DVBS2_INTERLEAVER_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Bit interleaves DVB-S2 FEC baseband frames.
     * \ingroup dtv
     *
     * Input: Normal or short FEC baseband frames with appended LPDC (LDPCFEC). \n
     * Output: Bit interleaved baseband frames.
     */
    class DTV_API dvbs2_interleaver_bb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<dvbs2_interleaver_bb> sptr;

      /*!
       * \brief Create a DVB-S2 bit interleaver.
       *
       * \param framesize FEC frame size (normal or short).
       * \param rate FEC code rate.
       * \param constellation DVB-S2 constellation.
       */
      static sptr make(dvb_framesize_t framesize, dvb_code_rate_t rate, dvb_constellation_t constellation);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBS2_INTERLEAVER_BB_H */

