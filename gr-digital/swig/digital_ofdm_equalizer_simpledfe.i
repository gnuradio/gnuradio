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


class digital_ofdm_equalizer_simpledfe;
typedef boost::shared_ptr<digital_ofdm_equalizer_simpledfe> digital_ofdm_equalizer_simpledfe_sptr;
%template(digital_ofdm_equalizer_simpledfe_sptr) boost::shared_ptr<digital_ofdm_equalizer_simpledfe>;
%rename(ofdm_equalizer_simpledfe) digital_make_ofdm_equalizer_simpledfe;
digital_ofdm_equalizer_simpledfe_sptr
digital_make_ofdm_equalizer_simpledfe(
      int fft_len,
      const digital_constellation_sptr &constellation,
      const std::vector<std::vector<int> > &occupied_carriers = std::vector<std::vector<int> >(),
      const std::vector<std::vector<int> > &pilot_carriers = std::vector<std::vector<int> >(),
      const std::vector<std::vector<gr_complex> > &pilot_symbols = std::vector<std::vector<gr_complex> >(),
      int symbols_skipped = 0,
      float alpha = 0.1,
      bool input_is_shifted = true);
%ignore digital_ofdm_equalizer_simpledfe;

class digital_ofdm_equalizer_simpledfe : public digital_ofdm_equalizer_1d_pilots
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

  void equalize(gr_complex *frame,
		  int n_sym,
		  const std::vector<gr_complex> &initial_taps = std::vector<gr_complex>(),
		  const std::vector<gr_tag_t> &tags = std::vector<gr_tag_t>());
  void get_channel_state(std::vector<gr_complex> &taps);
};

