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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/digital/ofdm_equalizer_static.h>

namespace gr {
  namespace digital {

    ofdm_equalizer_static::sptr
    ofdm_equalizer_static::make(
	  int fft_len,
	  const std::vector<std::vector<int> > &occupied_carriers,
	  const std::vector<std::vector<int> > &pilot_carriers,
	  const std::vector<std::vector<gr_complex> > &pilot_symbols,
	  int symbols_skipped,
	  bool input_is_shifted)
    {
      return ofdm_equalizer_static::sptr(
	  new ofdm_equalizer_static(
	      fft_len,
	      occupied_carriers,
	      pilot_carriers,
	      pilot_symbols,
	      symbols_skipped,
	      input_is_shifted
	  )
      );
    }

    ofdm_equalizer_static::ofdm_equalizer_static(
	int fft_len,
	const std::vector<std::vector<int> > &occupied_carriers,
	const std::vector<std::vector<int> > &pilot_carriers,
	const std::vector<std::vector<gr_complex> > &pilot_symbols,
	int symbols_skipped,
	bool input_is_shifted)
      : ofdm_equalizer_1d_pilots(fft_len, occupied_carriers, pilot_carriers, pilot_symbols, symbols_skipped, input_is_shifted)
    {
    }


    ofdm_equalizer_static::~ofdm_equalizer_static()
    {
    }


    void
    ofdm_equalizer_static::equalize(gr_complex *frame,
	      int n_sym,
	      const std::vector<gr_complex> &initial_taps,
	      const std::vector<tag_t> &tags)
    {
      d_channel_state = initial_taps;

      for (int i = 0; i < n_sym; i++) {
	for (int k = 0; k < d_fft_len; k++) {
	  if (!d_occupied_carriers[k]) {
	    continue;
	  }
	  if (!d_pilot_carriers.empty() && d_pilot_carriers[d_pilot_carr_set][k]) {
	    d_channel_state[k] = frame[i*d_fft_len + k] / d_pilot_symbols[d_pilot_carr_set][k];
	    frame[i*d_fft_len+k] = d_pilot_symbols[d_pilot_carr_set][k];
	  } else {
	    frame[i*d_fft_len+k] /= d_channel_state[k];
	  }
	}
	if (!d_pilot_carriers.empty()) {
	  d_pilot_carr_set = (d_pilot_carr_set + 1) % d_pilot_carriers.size();
	}
      }
    }

  } /* namespace digital */
} /* namespace gr */

