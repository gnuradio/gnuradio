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

#ifndef INCLUDED_DTV_DVBT_DVBT_DEMAP_H
#define INCLUDED_DTV_DVBT_DVBT_DEMAP_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt_config.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief DVB-T demapper.
     * \ingroup dtv
     *
     * ETSI EN 300 744 Clause 4.3.5. \n
     * Data input format: \n
     * complex(real(float), imag(float)). \n
     * Data output format: \n
     * 000000Y0Y1 - QPSK. \n
     * 0000Y0Y1Y2Y3 - 16QAM. \n
     * 00Y0Y1Y2Y3Y4Y5 - 64QAM.
     */
    class DTV_API dvbt_demap : virtual public block
    {
     public:
      typedef boost::shared_ptr<dvbt_demap> sptr;

      /*!
       * \brief Create a DVB-T demapper.
       *
       * \param nsize length of input stream. \n
       * \param constellation constellation used. \n
       * \param hierarchy hierarchy used. \n
       * \param transmission transmission mode used. \n
       * \param gain gain of complex input stream.
       */
      static sptr make(int nsize, dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, dvbt_transmission_mode_t transmission, float gain);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_DVBT_DEMAP_H */

