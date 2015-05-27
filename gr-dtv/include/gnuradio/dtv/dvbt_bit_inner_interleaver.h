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

#ifndef INCLUDED_DTV_DVBT_BIT_INNER_INTERLEAVER_H
#define INCLUDED_DTV_DVBT_BIT_INNER_INTERLEAVER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt_config.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Bit Inner interleaver.
     * \ingroup dtv
     *
     * ETSI EN 300 744 Clause 4.3.4.1 \n
     * Data Input format: \n
     * 000000X0X1 - QPSK. \n
     * 0000X0X1X2X3 - 16QAM. \n
     * 00X0X1X2X3X4X5 - 64QAM. \n
     * Data Output format: \n
     * 000000B0B1 - QPSK. \n
     * 0000B0B1B2B3 - 16QAM. \n
     * 00B0B1B2B3B4B5 - 64QAM. \n
     * bit interleaver block size is 126.
     */
    class DTV_API dvbt_bit_inner_interleaver : virtual public block
    {
     public:
      typedef boost::shared_ptr<dvbt_bit_inner_interleaver> sptr;

      /*!
       * \brief Create a Bit Inner interleaver
       *
       * \param nsize length of input stream. \n
       * \param constellation constellation used. \n
       * \param hierarchy hierarchy used. \n
       * \param transmission transmission mode used.
       */
      static sptr make(int nsize, dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, dvbt_transmission_mode_t transmission);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_BIT_INNER_INTERLEAVER_H */

