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

#ifndef INCLUDED_DIGITAL_OFDM_EQUALIZER_BASE_H
#define INCLUDED_DIGITAL_OFDM_EQUALIZER_BASE_H

#include <digital_api.h>
#include <gr_tags.h>
#include <gr_complex.h>
#include <boost/enable_shared_from_this.hpp>

class digital_ofdm_equalizer_base;
typedef boost::shared_ptr<digital_ofdm_equalizer_base> digital_ofdm_equalizer_base_sptr;

class digital_ofdm_equalizer_1d_pilots;
typedef boost::shared_ptr<digital_ofdm_equalizer_1d_pilots> digital_ofdm_equalizer_1d_pilots_sptr;

/* \brief Base class for implementation details of frequency-domain OFDM equalizers.
 * \ingroup ofdm_blk
 * \ingroup equalizers_blk
 * 
 */
class DIGITAL_API digital_ofdm_equalizer_base : public boost::enable_shared_from_this<digital_ofdm_equalizer_base>
{
 protected:
  int d_fft_len;
  int d_carr_offset;

 public:
  digital_ofdm_equalizer_base(int fft_len);
  ~digital_ofdm_equalizer_base();

  virtual void reset() = 0;
  void set_carrier_offset(int offset) { d_carr_offset = offset; };
  virtual void equalize(
		  gr_complex *frame,
		  int n_sym,
		  const std::vector<gr_complex> &initial_taps = std::vector<gr_complex>(),
		  const std::vector<gr_tag_t> &tags = std::vector<gr_tag_t>()) = 0;
  virtual void get_channel_state(std::vector<gr_complex> &taps) = 0;
  int fft_len() { return d_fft_len; };
  digital_ofdm_equalizer_base_sptr base() { return shared_from_this(); };
};


/* \brief Base class for implementation details of 1-dimensional OFDM FDEs which use pilot tones.
 * \ingroup digital
 *
 */
class DIGITAL_API digital_ofdm_equalizer_1d_pilots : public digital_ofdm_equalizer_base
{
 protected:
  //! If \p d_occupied_carriers[k][l] is true, symbol k, carrier l is carrying data.
  //  (this is a different format than occupied_carriers!)
  std::vector<bool> d_occupied_carriers;
  //! If \p d_pilot_carriers[k][l] is true, symbol k, carrier l is carrying data.
  //  (this is a different format than pilot_carriers!)
  std::vector<std::vector<bool> > d_pilot_carriers;
  //! If \p d_pilot_carriers[k][l] is true, d_pilot_symbols[k][l] is its tx'd value.
  //  (this is a different format than pilot_symbols!)
  std::vector<std::vector<gr_complex> > d_pilot_symbols;
  //! In case the frame doesn't begin with OFDM symbol 0, this is the index of the first symbol
  int d_symbols_skipped;
  //! The current position in the set of pilot symbols
  int d_pilot_carr_set;
  //! Vector of length d_fft_len saving the current channel state (on the occupied carriers)
  std::vector<gr_complex> d_channel_state;

 public:
  digital_ofdm_equalizer_1d_pilots(
      int fft_len,
      const std::vector<std::vector<int> > &occupied_carriers,
      const std::vector<std::vector<int> > &pilot_carriers,
      const std::vector<std::vector<gr_complex> > &pilot_symbols,
      int symbols_skipped,
      bool input_is_shifted);
  ~digital_ofdm_equalizer_1d_pilots();

  void reset();
  void get_channel_state(std::vector<gr_complex> &taps);
};

#endif /* INCLUDED_DIGITAL_OFDM_EQUALIZER_BASE_H */

