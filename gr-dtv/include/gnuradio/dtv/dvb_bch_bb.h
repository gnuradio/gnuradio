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


#ifndef INCLUDED_DTV_DVB_BCH_BB_H
#define INCLUDED_DTV_DVB_BCH_BB_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Encodes a BCH ((Bose, Chaudhuri, Hocquenghem) FEC.
     * \ingroup dtv
     *
     * \details
     * Input: Variable length FEC baseband frames (BBFRAME). \n
     * Output: Variable length FEC baseband frames with appended BCH (BCHFEC).
     */
    class DTV_API dvb_bch_bb : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<dvb_bch_bb> sptr;

      /*!
       * \brief Create a baseband frame BCH encoder.
       *
       * \param standard DVB standard (DVB-S2 or DVB-T2).
       * \param framesize FEC frame size (normal, medium or short).
       * \param rate FEC code rate.
       */
      static sptr make(dvb_standard_t standard, dvb_framesize_t framesize, dvb_code_rate_t rate);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVB_BCH_BB_H */

