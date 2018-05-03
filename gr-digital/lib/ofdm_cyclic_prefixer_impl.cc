/* -*- c++ -*- */
/*
 * Copyright 2013, 2018 Free Software Foundation, Inc.
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

#include <gnuradio/io_signature.h>
#include "ofdm_cyclic_prefixer_impl.h"

namespace gr {
  namespace digital {

    // Do not break backwards compatibility and overload the make function.
    ofdm_cyclic_prefixer::sptr
    ofdm_cyclic_prefixer::make(size_t input_size, size_t output_size, int rolloff_len, const std::string &len_tag_key)
    {
      int fft_len = input_size;
      std::vector<int> cp_lengths(1, static_cast<int>(output_size - input_size)); // Cast to silence compiler :(
      return gnuradio::get_initial_sptr(new ofdm_cyclic_prefixer_impl(fft_len, cp_lengths, rolloff_len, len_tag_key));
    }

    ofdm_cyclic_prefixer::sptr
    ofdm_cyclic_prefixer::make(int fft_len, std::vector<int> cp_lengths, int rolloff_len, const std::string &len_tag_key)
    {
      return gnuradio::get_initial_sptr(new ofdm_cyclic_prefixer_impl(fft_len, cp_lengths, rolloff_len, len_tag_key));
    }

    ofdm_cyclic_prefixer_impl::ofdm_cyclic_prefixer_impl(int fft_len, std::vector<int> cp_lengths, int rolloff_len, const std::string &len_tag_key)
      : gr::tagged_stream_block
        (
          "ofdm_cyclic_prefixer",
          gr::io_signature::make(1, 1, fft_len*sizeof(gr_complex)),
          gr::io_signature::make(1, 1, sizeof(gr_complex)), len_tag_key
        ),
        d_fft_len(fft_len),
        d_state(0),
        d_cp_max(0),
        d_cp_min(std::numeric_limits<int>::max()),
        d_rolloff_len(rolloff_len),
        d_cp_lengths(cp_lengths),
        d_up_flank((rolloff_len ? rolloff_len-1 : 0), 0),
        d_down_flank((rolloff_len ? rolloff_len-1 : 0), 0),
        d_delay_line(0, 0),
        d_len_tag_key(len_tag_key)
    {
      // Sanity
      if (d_cp_lengths.empty()) {
        throw std::invalid_argument(this->alias() + std::string(": CP lengths vector can not be empty."));
      }
      for(unsigned i=0; i<d_cp_lengths.size(); i++) {
        if (d_cp_lengths[i] != 0) {
          break;
        }
        if (i == d_cp_lengths.size()-1) {
          throw std::invalid_argument(this->alias() + std::string(": Please provide at least one CP which is != 0."));
        }
      }
      for(unsigned i=0; i<d_cp_lengths.size(); i++) {
        d_cp_max = std::max(d_cp_max, d_cp_lengths[i]);
        d_cp_min = std::min(d_cp_min, d_cp_lengths[i]);
      }
      if (d_cp_min < 0) {
        throw std::invalid_argument(this->alias() + std::string(": The minimum CP allowed is 0."));
      }
      // Give the buffer allocator and scheduler a hint about the ratio between input and output.
      set_relative_rate(d_cp_max + d_fft_len);
      // Flank of length 1 would just be rectangular.
      if(d_rolloff_len == 1) {
        d_rolloff_len = 0;
        GR_LOG_WARN(d_logger, "Set rolloff to 0, because 1 would result in a boxcar function.");
      }
      if(d_rolloff_len) {
        d_delay_line.resize(d_rolloff_len-1, 0);
        // More sanity
        if(d_rolloff_len > d_cp_min) {
          throw std::invalid_argument(this->alias() + std::string(": Rolloff length must be smaller than any of the cyclic prefix lengths."));
        }
        /* The actual flanks are one sample shorter than d_rolloff_len, because the
           first sample of the up- and down flank is always zero and one, respectively.*/
        for(int i = 1; i < d_rolloff_len; i++) {
          d_up_flank[i-1]   = 0.5 * (1 + cos(M_PI * i/rolloff_len - M_PI));
          d_down_flank[i-1] = 0.5 * (1 + cos(M_PI * (rolloff_len-i)/rolloff_len - M_PI));
        }
      }
      if(d_len_tag_key.empty()) {
        // noutput_items is set to be a multiple of the largest possible output size.
        // It is always OK to return less (in case of the shorter CP).
        set_output_multiple(d_fft_len + d_cp_max);
      }
      else {
        // Avoid automatic tag propagation and propagate them manually.
        set_tag_propagation_policy(TPP_DONT);
      }
    }

    ofdm_cyclic_prefixer_impl::~ofdm_cyclic_prefixer_impl()
    {
    }

    int
    ofdm_cyclic_prefixer_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int noutput_items = ninput_items[0] * (d_cp_max + d_fft_len) + ( d_len_tag_key.empty() ? 0 : d_delay_line.size() );
      return noutput_items;
    }

    int
    ofdm_cyclic_prefixer_impl::work
    (
      int noutput_items,
      gr_vector_int &ninput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items
    )
    {
      gr_complex *in = (gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      int symbols_to_read = 0;
      // 1) Figure out if we're in freewheeling or packet mode.
      if(!d_len_tag_key.empty()) {
        symbols_to_read = ninput_items[0];
      } else {
        symbols_to_read = std::min(noutput_items / (int)(d_fft_len + d_cp_max), ninput_items[0]);
      }
      noutput_items = 0;
      // 2) Do the cyclic prefixing and, optionally, the pulse shaping.
      for(int sym_idx = 0; sym_idx < symbols_to_read; sym_idx++) {
        memcpy(static_cast<void*>(out + d_cp_lengths[d_state]), static_cast<void*>(in), d_fft_len * sizeof(gr_complex));
        memcpy(static_cast<void*>(out), static_cast<void*>(in + d_fft_len - d_cp_lengths[d_state]), d_cp_lengths[d_state] * sizeof(gr_complex));
        if(d_rolloff_len) {
          for(int i = 0; i < d_rolloff_len-1; i++) {
            out[i] = out[i] * d_up_flank[i] + d_delay_line[i];
            /* This is basically a cyclic suffix, but completely shifted into the next symbol.
               The data rate does not change. */
            d_delay_line[i] = in[i] * d_down_flank[i];
          }
        }
        in += d_fft_len;
        out += d_fft_len + d_cp_lengths[d_state];
        // Raise the number of noutput_items depending on how long the current output was.
        noutput_items += d_fft_len + d_cp_lengths[d_state];
        // Propagate tags.
        unsigned last_state = d_state > 0 ? d_state-1 : d_cp_lengths.size()-1;
        std::vector<tag_t> tags;
        get_tags_in_range(tags, 0, nitems_read(0) + sym_idx, nitems_read(0) + sym_idx + 1);
        for(unsigned i = 0; i < tags.size(); i++) {
          tags[i].offset = ( (tags[i].offset - nitems_read(0)) * (d_fft_len + d_cp_lengths[last_state]) ) + nitems_written(0);
          add_item_tag(0, tags[i].offset, tags[i].key, tags[i].value);
        }
        // Finally switch to next state.
        ++d_state %= d_cp_lengths.size();
      }
      /* 3) If we're in packet mode:
            - flush the delay line, if applicable */
      if(!d_len_tag_key.empty()) {
        if(d_rolloff_len) {
          std::memcpy(static_cast<void*>(out), static_cast<void*>(d_delay_line.data()), sizeof(gr_complex)*d_delay_line.size());
          d_delay_line.assign(d_delay_line.size(), 0);
          // Make last symbol a bit longer.
          noutput_items += d_delay_line.size();
        }
      }
      else {
        consume_each(symbols_to_read);
      }
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
  } // namespace digital
} // namespace gr
