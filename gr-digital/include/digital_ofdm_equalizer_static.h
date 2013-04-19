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

#ifndef INCLUDED_DIGITAL_OFDM_EQUALIZER_STATIC_H
#define INCLUDED_DIGITAL_OFDM_EQUALIZER_STATIC_H

#include <digital_api.h>
#include <digital_constellation.h>
#include <digital_ofdm_equalizer_base.h>

class digital_ofdm_equalizer_static;
typedef boost::shared_ptr<digital_ofdm_equalizer_static> digital_ofdm_equalizer_static_sptr;

/*
 * \param fft_len FFT length
 * \param occupied_carriers List of occupied carriers, see ofdm_carrier_allocator
 *                          for a description.
 * \param pilot_carriers Position of pilot symbols, see ofdm_carrier_allocator
 *                          for a description.
 * \param pilot_symbols Value of pilot symbols, see ofdm_carrier_allocator
 *                          for a description.
 * \param symbols_skipped Starting position within occupied_carriers and pilot_carriers.
 *                        If the first symbol of the frame was removed (e.g. to decode the
 *                        header), set this make sure the pilot symbols are correctly
 *                        identified.
 * \param input_is_shifted Set this to false if the input signal is not shifted, i.e.
 *                         the first input items is on the DC carrier.
 *                         Note that a lot of the OFDM receiver blocks operate on shifted
 *                         signals!
 */
DIGITAL_API digital_ofdm_equalizer_static_sptr
digital_make_ofdm_equalizer_static(
      int fft_len,
      const std::vector<std::vector<int> > &occupied_carriers = std::vector<std::vector<int> >(),
      const std::vector<std::vector<int> > &pilot_carriers = std::vector<std::vector<int> >(),
      const std::vector<std::vector<gr_complex> > &pilot_symbols = std::vector<std::vector<gr_complex> >(),
      int symbols_skipped = 0,
      bool input_is_shifted = true);

/* \brief Very simple static equalizer for OFDM.
 * \ingroup ofdm_blk
 * \ingroup equalizers_blk
 *
 * This is an extremely simple equalizer. It will only work for high-SNR, very, very
 * slowly changing channels.
 * It simply divides the signal with the currently known channel state. Whenever
 * a pilot symbol comes around, it updates the channel state on that particular
 * carrier by dividing the received symbol with the known pilot symbol.
 */
class DIGITAL_API digital_ofdm_equalizer_static : public digital_ofdm_equalizer_1d_pilots
{
 public:
  digital_ofdm_equalizer_static(
      int fft_len,
      const std::vector<std::vector<int> > &occupied_carriers,
      const std::vector<std::vector<int> > &pilot_carriers,
      const std::vector<std::vector<gr_complex> > &pilot_symbols,
      int symbols_skipped,
      bool input_is_shifted);
  ~digital_ofdm_equalizer_static();

  /*! \brief Divide the input signal with the current channel state.
   *
   * Does the following (and nothing else):
   * - Divide every OFDM symbol with the current channel state
   * - If a pilot symbol is found, re-set the channel state by dividing the received
   *   symbol with the known pilot symbol
   */
  void equalize(gr_complex *frame,
		  int n_sym,
		  const std::vector<gr_complex> &initial_taps = std::vector<gr_complex>(),
		  const std::vector<gr_tag_t> &tags = std::vector<gr_tag_t>());

 private:

};

#endif /* INCLUDED_DIGITAL_OFDM_EQUALIZER_STATIC_H */

