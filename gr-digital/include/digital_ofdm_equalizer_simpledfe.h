/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_OFDM_EQUALIZER_SIMPLEDFE_H
#define INCLUDED_DIGITAL_OFDM_EQUALIZER_SIMPLEDFE_H

#include <digital_api.h>
#include <digital_constellation.h>
#include <digital_ofdm_equalizer_base.h>

class digital_ofdm_equalizer_simpledfe;
typedef boost::shared_ptr<digital_ofdm_equalizer_simpledfe> digital_ofdm_equalizer_simpledfe_sptr;

/*
 * \param fft_len FFT length
 * \param constellation The constellation object describing the modulation used
 *                      on the subcarriers (e.g. QPSK). This is used to decode
 *                      the individual symbols.
 * \param occupied_carriers List of occupied carriers, see ofdm_carrier_allocator
 *                          for a description.
 * \param pilot_carriers Position of pilot symbols, see ofdm_carrier_allocator
 *                          for a description.
 * \param pilot_symbols Value of pilot symbols, see ofdm_carrier_allocator
 *                          for a description.
 * \param alpha Averaging coefficient (in a nutshell, if \f$H_{i,k}\f$ is the channel
 *              state for carrier i and symbol k,
 *              \f$H_{i,k+1} =  \alpha H_{i,k} + (1 - \alpha) H_{i,k+1}\f$. Make this
 *              larger if there's more noise, but keep in mind that larger values
 *              of alpha mean slower response to channel changes).
 * \param symbols_skipped Starting position within occupied_carriers and pilot_carriers.
 *                        If the first symbol of the frame was removed (e.g. to decode the
 *                        header), set this make sure the pilot symbols are correctly
 *                        identified.
 * \param input_is_shifted Set this to false if the input signal is not shifted, i.e.
 *                         the first input items is on the DC carrier.
 *                         Note that a lot of the OFDM receiver blocks operate on shifted
 *                         signals!
 */
DIGITAL_API digital_ofdm_equalizer_simpledfe_sptr
digital_make_ofdm_equalizer_simpledfe(
      int fft_len,
      const digital_constellation_sptr &constellation,
      const std::vector<std::vector<int> > &occupied_carriers = std::vector<std::vector<int> >(),
      const std::vector<std::vector<int> > &pilot_carriers = std::vector<std::vector<int> >(),
      const std::vector<std::vector<gr_complex> > &pilot_symbols = std::vector<std::vector<gr_complex> >(),
      int symbols_skipped = 0,
      float alpha = 0.1,
      bool input_is_shifted = true);

/* \brief Simple decision feedback equalizer for OFDM.
 * \ingroup ofdm_blk
 * \ingroup equalizers_blk
 *
 * Equalizes an OFDM signal symbol by symbol using knowledge of the
 * complex modulations symbols.
 * For every symbol, the following steps are performed:
 * - On every sub-carrier, decode the modulation symbol
 * - Use the difference between the decoded symbol and the received symbol
 *   to update the channel state on this carrier
 * - Whenever a pilot symbol is found, it uses the known pilot symbol to
 *   update the channel state.
 *
 * This equalizer makes a lot of assumptions:
 * - The initial channel state is good enough to decode the first
 *   symbol without error (unless the first symbol only consists of pilot
 *   tones)
 * - The channel changes only very slowly, such that the channel state
 *   from one symbol is enough to decode the next
 * - SNR low enough that equalization will always suffice to correctly
 *   decode a symbol
 * If these assumptions are not met, the most common error is that the
 * channel state is estimated incorrectly during equalization; after that,
 * all subsequent symbols will be completely wrong.
 *
 * Note that the equalized symbols are *exact points* on the constellation.
 * This means soft information of the modulation symbols is lost after the
 * equalization, which is suboptimal for channel codes that use soft decision.
 *
 */
class DIGITAL_API digital_ofdm_equalizer_simpledfe : public digital_ofdm_equalizer_1d_pilots
{
 public:
  digital_ofdm_equalizer_simpledfe(
      int fft_len,
      const digital_constellation_sptr &constellation,
      const std::vector<std::vector<int> > &occupied_carriers = std::vector<std::vector<int> >(),
      const std::vector<std::vector<int> > &pilot_carriers = std::vector<std::vector<int> >(),
      const std::vector<std::vector<gr_complex> > &pilot_symbols = std::vector<std::vector<gr_complex> >(),
      int symbols_skipped = 0,
      float alpha = 0.1,
      bool input_is_shifted = true);

  ~digital_ofdm_equalizer_simpledfe();

  void equalize(gr_complex *frame,
		  int n_sym,
		  const std::vector<gr_complex> &initial_taps = std::vector<gr_complex>(),
		  const std::vector<gr_tag_t> &tags = std::vector<gr_tag_t>());

 private:
  digital_constellation_sptr d_constellation;
  //! Averaging coefficient
  float d_alpha;

};

#endif /* INCLUDED_DIGITAL_OFDM_EQUALIZER_SIMPLEDFE_H */

