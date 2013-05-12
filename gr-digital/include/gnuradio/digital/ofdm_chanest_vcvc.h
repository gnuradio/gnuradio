/* -*- c++ -*- */
/* 
 * Copyright 2013 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DIGITAL_OFDM_CHANEST_VCVC_H
#define INCLUDED_DIGITAL_OFDM_CHANEST_VCVC_H

#include <gnuradio/digital/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace digital {

    /*!
     * \brief Estimate channel and coarse frequency offset for OFDM from preambles
     * \ingroup ofdm_blk
     * \ingroup syncronizers_blk
     *
     * Input: OFDM symbols (in frequency domain). The first one (or two) symbols are expected
     *        to be synchronisation symbols, which are used to estimate the coarse freq offset
     *        and the initial equalizer taps (these symbols are removed from the stream).
     *        The following \p n_data_symbols are passed through unmodified (the actual equalisation
     *        must be done elsewhere).
     * Output: The data symbols, without the synchronisation symbols.
     *         The first data symbol passed through has two tags:
     *         'ofdm_sync_carr_offset' (integer), the coarse frequency offset as number of carriers,
     *         and 'ofdm_sync_eq_taps' (complex vector).
     *         Any tags attached to the synchronisation symbols are attached to the first data
     *         symbol. All other tags are propagated as expected.
     *
     * Note: The vector on ofdm_sync_eq_taps is already frequency-corrected, whereas the rest is not.
     *
     * This block assumes the frequency offset is even (i.e. an integer multiple of 2).
     *
     * [1] Schmidl, T.M. and Cox, D.C., "Robust frequency and timing synchronization for OFDM",
     *     Communications, IEEE Transactions on, 1997.
     * [2] K.D. Kammeyer, "Nachrichtenuebertragung," Chapter. 16.3.2.
     */
    class DIGITAL_API ofdm_chanest_vcvc : virtual public block
    {
     public:
      typedef boost::shared_ptr<ofdm_chanest_vcvc> sptr;

      /*!
       * \param sync_symbol1 First synchronisation symbol in the frequency domain. Its length must be
       *                     the FFT length. For Schmidl & Cox synchronisation, every second sub-carrier
       *                     has to be zero.
       * \param sync_symbol2 Second synchronisation symbol in the frequency domain. Must be equal to
       *                     the FFT length, or zero length if only one synchronisation symbol is used.
       *                     Using this symbol is how synchronisation is described in [1]. Leaving this
       *                     empty forces us to interpolate the equalizer taps.
       *                     If you are using an unusual sub-carrier configuration (e.g. because of OFDMA),
       *                     this sync symbol is used to identify the active sub-carriers. If you only
       *                     have one synchronisation symbol, set the active sub-carriers to a non-zero
       *                     value in here, and also set \p force_one_sync_symbol parameter to true.
       * \param n_data_symbols The number of data symbols following each set of synchronisation symbols.
       *                       Must be at least 1.
       * \param eq_noise_red_len If non-zero, noise reduction for the equalizer taps is done according
       *                         to [2]. In this case, it is the channel influence time in number of
       *                         samples. A good value is usually the length of the cyclic prefix.
       * \param max_carr_offset Limit the number of sub-carriers the frequency offset can maximally be.
       *                        Leave this zero to try all possibilities.
       * \param force_one_sync_symbol See \p sync_symbol2.
       */
      static sptr make(
	  const std::vector<gr_complex> &sync_symbol1,
	  const std::vector<gr_complex> &sync_symbol2,
	  int n_data_symbols,
	  int eq_noise_red_len=0,
	  int max_carr_offset=-1,
	  bool force_one_sync_symbol=false
      );
    };

  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_OFDM_CHANEST_VCVC_H */

