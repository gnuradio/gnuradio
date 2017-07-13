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

#ifndef INCLUDED_DTV_DVBT2_P1INSERTION_CC_H
#define INCLUDED_DTV_DVBT2_P1INSERTION_CC_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/dtv/dvb_config.h>
#include <gnuradio/dtv/dvbt2_config.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief Inserts a P1 symbol.
     * \ingroup dtv
     *
     * Input: OFDM T2 frame. \n
     * Output: OFDM T2 frame with P1 symbol.
     */
    class DTV_API dvbt2_p1insertion_cc : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<dvbt2_p1insertion_cc> sptr;

      /*!
       * \brief Create a P1 symbol inserter.
       *
       * \param carriermode number of carriers (normal or extended).
       * \param fftsize OFDM IFFT size.
       * \param guardinterval OFDM ISI guard interval.
       * \param numdatasyms number of OFDM symbols in a T2 frame.
       * \param preamble P1 symbol preamble format.
       * \param showlevels print peak IQ levels.
       * \param vclip set peak IQ level threshold.
       */
      static sptr make(dvbt2_extended_carrier_t carriermode, dvbt2_fftsize_t fftsize, dvb_guardinterval_t guardinterval, int numdatasyms, dvbt2_preamble_t preamble, dvbt2_showlevels_t showlevels, float vclip);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT2_P1INSERTION_CC_H */

