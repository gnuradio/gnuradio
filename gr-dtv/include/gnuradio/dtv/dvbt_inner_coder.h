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

#ifndef INCLUDED_DTV_DVBT_INNER_CODER_H
#define INCLUDED_DTV_DVBT_INNER_CODER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt_config.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Inner coder with Puncturing.
     * \ingroup dtv
     *
     * ETSI EN 300 744 Clause 4.3.3 \n
     * Mother convolutional code with rate 1/2. \n
     * k=1, n=2, K=6. \n
     * Generator polynomial G1=171(OCT), G2=133(OCT). \n
     * Punctured to obtain rates of 2/3, 3/4, 5/6, 7/8. \n
     * Data Input format: Packed bytes (each bit is data). \n
     * MSB - first, LSB last. \n
     * Data Output format: \n
     * 000000X0X1 - QPSK. \n
     * 0000X0X1X2X3 - 16QAM. \n
     * 00X0X1X2X3X4X5 - 64QAM.
     */
    class DTV_API dvbt_inner_coder : virtual public block
    {
     public:
      typedef boost::shared_ptr<dvbt_inner_coder> sptr;

      /*!
       * \brief Create an Inner coder with Puncturing.
       *
       * \param ninput length of input. \n
       * \param noutput length of output. \n
       * \param constellation type of constellation. \n
       * \param hierarchy type of hierarchy used. \n
       * \param coderate coderate used.
       */
      static sptr make(int ninput, int noutput, dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, dvb_code_rate_t coderate);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_INNER_CODER_H */

