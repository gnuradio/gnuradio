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



class digital_ofdm_equalizer_base;
typedef boost::shared_ptr<digital_ofdm_equalizer_base> digital_ofdm_equalizer_base_sptr;
%template(digital_ofdm_equalizer_base_sptr) boost::shared_ptr<digital_ofdm_equalizer_base>;
%ignore digital_ofdm_equalizer_base;

class digital_ofdm_equalizer_1d_pilots;
typedef boost::shared_ptr<digital_ofdm_equalizer_1d_pilots> digital_ofdm_equalizer_1d_pilots_sptr;
%template(digital_ofdm_equalizer_1d_pilots_sptr) boost::shared_ptr<digital_ofdm_equalizer_1d_pilots>;
%ignore digital_ofdm_equalizer_1d_pilots;

class digital_ofdm_equalizer_base
{
 public:
  digital_ofdm_equalizer_base(int fft_len);

  virtual void reset() = 0;
  virtual void equalize(
		  gr_complex *frame,
		  int n_sym,
		  const std::vector<gr_complex> &initial_taps = std::vector<gr_complex>(),
		  const std::vector<gr_tag_t> &tags = std::vector<gr_tag_t>()) = 0;
  virtual void get_channel_state(std::vector<gr_complex> &taps) = 0;
  digital_ofdm_equalizer_base_sptr base();
};


class digital_ofdm_equalizer_1d_pilots : public digital_ofdm_equalizer_base
{
 public:
  digital_ofdm_equalizer_1d_pilots(
      int fft_len,
      const std::vector<std::vector<int> > &occupied_carriers,
      const std::vector<std::vector<int> > &pilot_carriers,
      const std::vector<std::vector<gr_complex> > &pilot_symbols,
      int symbols_skipped,
      bool input_is_shifted);

  void reset();
};

