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

#ifndef INCLUDED_DTV_DVBT_OFDM_SYM_ACQUISITION_H
#define INCLUDED_DTV_DVBT_OFDM_SYM_ACQUISITION_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace dtv {

    /*!
     * \brief OFDM symbol acquisition.
     * \ingroup dtv
     *
     * Data input format: \n
     * complex(real(float), imag(float)). \n
     * Data output format: \n
     * complex(real(float), imag(float)).
     */
    class DTV_API dvbt_ofdm_sym_acquisition : virtual public block
    {
     public:
      typedef boost::shared_ptr<dvbt_ofdm_sym_acquisition> sptr;

      /*!
       * \brief Create OFDM symbol acquisition.
       *
       * \param blocks Always equal to 1.\n
       * \param fft_length FFT size, 2048 or 8192. \n
       * \param occupied_tones Active OFDM carriers, 1705 or 6817. \n
       * \param cp_length Length of Cyclic Prefix (FFT size / 32, 16, 8 or 4). \n
       * \param snr Initial Signal to Noise Ratio.
       */
      static sptr make(int blocks, int fft_length, int occupied_tones, int cp_length, float snr);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_OFDM_SYM_ACQUISITION_H */

