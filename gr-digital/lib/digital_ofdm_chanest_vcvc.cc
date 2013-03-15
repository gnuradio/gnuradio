/* -*- c++ -*- */
// vim: set sw=2:
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

#include <gr_io_signature.h>
#include "digital_ofdm_chanest_vcvc.h"

digital_ofdm_chanest_vcvc_sptr
digital_make_ofdm_chanest_vcvc (
		const std::vector<gr_complex> &sync_symbol1,
		const std::vector<gr_complex> &sync_symbol2,
		int n_data_symbols,
		int eq_noise_red_len,
		int max_carr_offset,
		bool force_one_sync_symbol)
{
	return gnuradio::get_initial_sptr (new digital_ofdm_chanest_vcvc(
				sync_symbol1, sync_symbol2, n_data_symbols, eq_noise_red_len, max_carr_offset, force_one_sync_symbol));
}


digital_ofdm_chanest_vcvc::digital_ofdm_chanest_vcvc (
		const std::vector<gr_complex> &sync_symbol1,
		const std::vector<gr_complex> &sync_symbol2,
		int n_data_symbols,
		int eq_noise_red_len,
		int max_carr_offset,
		bool force_one_sync_symbol)
  : gr_block ("ofdm_chanest_vcvc",
		   gr_make_io_signature(1, 1, sizeof (gr_complex) * sync_symbol1.size()),
		   gr_make_io_signature(1, 1, sizeof (gr_complex) * sync_symbol1.size())),
  d_fft_len(sync_symbol1.size()),
  d_n_data_syms(n_data_symbols),
  d_n_sync_syms(1),
  d_eq_noise_red_len(eq_noise_red_len),
  d_ref_sym((sync_symbol2.size() && !force_one_sync_symbol) ? sync_symbol2 : sync_symbol1),
  d_corr_v(sync_symbol2),
  d_known_symbol_diffs(0, 0),
  d_new_symbol_diffs(0, 0),
  d_interpolate(false)
{
  // Set index of first and last active carrier
  for (int i = 0; i < d_fft_len; i++) {
    if (d_ref_sym[i] != gr_complex(0, 0)) {
      d_first_active_carrier = i;
      break;
    }
  }
  for (int i = d_fft_len-1; i >= 0; i--) {
    if (d_ref_sym[i] != gr_complex(0, 0)) {
      d_last_active_carrier = i;
      break;
    }
  }

  // Sanity checks
  if (sync_symbol2.size()) {
    if (sync_symbol1.size() != sync_symbol2.size()) {
      throw std::invalid_argument("sync symbols must have equal length.");
    }
    if (!force_one_sync_symbol) {
      d_n_sync_syms = 2;
    }
  } else {
    if (sync_symbol1[d_first_active_carrier+1] == gr_complex(0, 0)) {
      d_last_active_carrier++;
      d_interpolate = true;
    }
  }

  // Set up coarse freq estimation info
  // Allow all possible values:
  d_max_neg_carr_offset = -d_first_active_carrier;
  d_max_pos_carr_offset = d_fft_len - d_last_active_carrier - 1;
  if (max_carr_offset != -1) {
    d_max_neg_carr_offset = std::max(-max_carr_offset, d_max_neg_carr_offset);
    d_max_pos_carr_offset = std::min(max_carr_offset, d_max_pos_carr_offset);
  }
  // Carrier offsets must be even
  if (d_max_neg_carr_offset % 2)
    d_max_neg_carr_offset++;
  if (d_max_pos_carr_offset % 2)
    d_max_pos_carr_offset--;

  if (d_n_sync_syms == 2) {
    for (int i = 0; i < d_fft_len; i++) {
      if (sync_symbol1[i] == gr_complex(0, 0)) {
	d_corr_v[i] = gr_complex(0, 0);
      } else {
	d_corr_v[i] /= sync_symbol1[i];
      }
    }
  } else {
    d_corr_v.resize(0, 0);
    d_known_symbol_diffs.resize(d_fft_len, 0);
    d_new_symbol_diffs.resize(d_fft_len, 0);
    for (int i = d_first_active_carrier; i < d_last_active_carrier-2 && i < d_fft_len-2; i += 2) {
      d_known_symbol_diffs[i] = std::norm(sync_symbol1[i] - sync_symbol1[i+2]);
    }
  }

  set_relative_rate((double) n_data_symbols / (n_data_symbols + d_n_sync_syms));
  set_tag_propagation_policy(TPP_DONT);
}


digital_ofdm_chanest_vcvc::~digital_ofdm_chanest_vcvc()
{
}

void
digital_ofdm_chanest_vcvc::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  ninput_items_required[0] = (noutput_items/d_n_data_syms) * (d_n_data_syms + d_n_sync_syms);
}


int
digital_ofdm_chanest_vcvc::get_carr_offset(const gr_complex *sync_sym1, const gr_complex *sync_sym2)
{
  int carr_offset = 0;
  if (d_corr_v.size()) {
    // Use Schmidl & Cox method
    float Bg_max = 0;
    // g here is 2g in the paper
    for (int g = d_max_neg_carr_offset; g <= d_max_pos_carr_offset; g += 2) {
      gr_complex tmp = gr_complex(0, 0);
      for (int k = 0; k < d_fft_len; k++) {
	if (d_corr_v[k] != gr_complex(0, 0)) {
	  tmp += std::conj(sync_sym1[k+g]) * std::conj(d_corr_v[k]) * sync_sym2[k+g];
	}
      }
      if (std::abs(tmp) > Bg_max) {
	Bg_max = std::abs(tmp);
	carr_offset = g;
      }
    }
  } else {
    // Correlate
    std::fill(d_new_symbol_diffs.begin(), d_new_symbol_diffs.end(), 0);
    for(int i = 0; i < d_fft_len-2; i++) {
      d_new_symbol_diffs[i] = std::norm(sync_sym1[i] - sync_sym1[i+2]);
    }

    float sum;
    float max = 0;
    for (int g = d_max_neg_carr_offset; g <= d_max_pos_carr_offset; g += 2) {
      sum = 0;
      for (int j = 0; j < d_fft_len; j++) {
	if (d_known_symbol_diffs[j]) {
	  sum += (d_known_symbol_diffs[j] * d_new_symbol_diffs[j + g]);
	}
	if(sum > max) {
	  max = sum;
	  carr_offset = g;
	}
      }
    }
  }
  return carr_offset;
}


void
digital_ofdm_chanest_vcvc::get_chan_taps(
    const gr_complex *sync_sym1,
    const gr_complex *sync_sym2,
    int carr_offset,
    std::vector<gr_complex> &taps)
{
  const gr_complex *sym = ((d_n_sync_syms == 2) ? sync_sym2 : sync_sym1);
  std::fill(taps.begin(), taps.end(), gr_complex(0, 0));
  int loop_start = 0;
  int loop_end = d_fft_len;
  if (carr_offset > 0) {
    loop_start = carr_offset;
  } else if (carr_offset < 0) {
    loop_end = d_fft_len + carr_offset;
  }
  for (int i = loop_start; i < loop_end; i++) {
    if ((d_ref_sym[i-carr_offset] != gr_complex(0, 0))) {
      taps[i] = sym[i] / d_ref_sym[i-carr_offset];
    }
  }

  if (d_interpolate) {
    for (int i = d_first_active_carrier + 1; i < d_last_active_carrier; i += 2) {
      taps[i] = (taps[i-1] + taps[i+1]) / gr_complex(2.0, 0);
    }
    taps[d_last_active_carrier] = taps[d_last_active_carrier-1];
  }

  if (d_eq_noise_red_len) {
    // TODO
    // 1) IFFT
    // 2) Set all elements > d_eq_noise_red_len to zero
    // 3) FFT
  }
}


// 1) Go through all the frames available on the input buffer
// 2) Estimate the coarse freq. offset and the eq. taps from the
//    input symbol(s)
// 3) Copy the data symbols to the output
// 4) Copy all other tags onto the output. A tag that was on
//    a sync symbol is copied onto the first data symbol.
// 5) Add the new tags for carrier offset and eq. taps
int
digital_ofdm_chanest_vcvc::general_work (int noutput_items,
				   gr_vector_int &ninput_items,
				   gr_vector_const_void_star &input_items,
				   gr_vector_void_star &output_items)
{
  const gr_complex *in = (const gr_complex *) input_items[0];
  gr_complex *out = (gr_complex *) output_items[0];
  int n_frames = noutput_items/d_n_data_syms;
  const int framesize = d_n_sync_syms + d_n_data_syms;

  for (int i = 0; i < n_frames; i++) {
    int carr_offset = 0;
    if (d_max_neg_carr_offset || d_max_pos_carr_offset)
      carr_offset = get_carr_offset(in, in+d_fft_len);
    std::vector<gr_complex> chan_taps(d_fft_len, 0);
    get_chan_taps(in, in+d_fft_len, carr_offset, chan_taps);

    memcpy((void *) out,
	   (void *) &in[d_n_sync_syms * d_fft_len],
	   sizeof(gr_complex) * d_fft_len * d_n_data_syms);
    in += framesize * d_fft_len;
    out += d_n_data_syms * d_fft_len;

    std::vector<gr_tag_t> tags;
    this->get_tags_in_range(tags, 0,
	this->nitems_read(0)+i*framesize,
	this->nitems_read(0)+(i+1)*framesize);
    for (unsigned t = 0; t < tags.size(); t++) {
      int offset = tags[t].offset - (this->nitems_read(0) + i*framesize);
      if (offset < d_n_sync_syms) {
	offset = 0;
      } else {
	offset -= d_n_sync_syms;
      }
      tags[t].offset = offset + this->nitems_written(0) + i*d_n_data_syms;
      this->add_item_tag(0, tags[t]);
    }

    this->add_item_tag(0, this->nitems_written(0) + i*d_n_data_syms,
	pmt::pmt_string_to_symbol("ofdm_sync_carr_offset"),
	pmt::pmt_from_long(carr_offset));
    this->add_item_tag(0, this->nitems_written(0) + i*d_n_data_syms,
	pmt::pmt_string_to_symbol("ofdm_sync_chan_taps"),
	pmt::pmt_init_c32vector(d_fft_len, chan_taps));
  }

  consume_each(n_frames * framesize);
  return n_frames * d_n_data_syms;
}

