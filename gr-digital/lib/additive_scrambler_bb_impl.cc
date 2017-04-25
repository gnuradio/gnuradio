/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
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

#include "additive_scrambler_bb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace digital {

    additive_scrambler_bb::sptr
    additive_scrambler_bb::make (int mask, int seed,
				 int len, int count,
				 int bits_per_byte,
				 const std::string &reset_tag_key)
    {
      return gnuradio::get_initial_sptr(new additive_scrambler_bb_impl
					(mask, seed, len, count, bits_per_byte, reset_tag_key));
    }

    additive_scrambler_bb_impl::additive_scrambler_bb_impl(int mask,
							   int seed,
							   int len,
							   int count,
							   int bits_per_byte,
							   const std::string &reset_tag_key)
      : sync_block("additive_scrambler_bb",
		      io_signature::make(1, 1, sizeof(unsigned char)),
		      io_signature::make(1, 1, sizeof(unsigned char))),
	d_lfsr(mask, seed, len),
	d_count(reset_tag_key.empty() ? count : -1),
	d_bytes(0), d_len(len), d_seed(seed),
	d_bits_per_byte(bits_per_byte), d_reset_tag_key(pmt::string_to_symbol(reset_tag_key))
    {
      if (d_count < -1) {
      	throw std::invalid_argument("count must be non-negative!");
      }
      if (d_bits_per_byte < 1 || d_bits_per_byte > 8) {
      	throw std::invalid_argument("bits_per_byte must be in [1, 8]");
      }
    }

    additive_scrambler_bb_impl::~additive_scrambler_bb_impl()
    {
    }

    int
    additive_scrambler_bb_impl::mask() const
    {
      return d_lfsr.mask();
    }

    int
    additive_scrambler_bb_impl::seed() const
    {
      return d_seed;
    }

    int
    additive_scrambler_bb_impl::len() const
    {
      return d_len;
    }

    int
    additive_scrambler_bb_impl::count() const
    {
      return d_count;
    }

    int additive_scrambler_bb_impl::_get_next_reset_index(int noutput_items, int last_reset_index /* = -1 */)
    {
      int reset_index = noutput_items; // This is a value that gets never reached in the for loop
      if (d_count == -1) {
	std::vector<gr::tag_t> tags;
	get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+noutput_items, d_reset_tag_key);
	for (unsigned i = 0; i < tags.size(); i++) {
	  int reset_pos = tags[i].offset - nitems_read(0);
	  if (reset_pos < reset_index && reset_pos > last_reset_index) {
	    reset_index = reset_pos;
	  };
	}
      } else {
	if (last_reset_index == -1) {
	  reset_index = d_count - d_bytes;
	} else {
	  reset_index = last_reset_index + d_count;
	}
      }
      return reset_index;
    }

    int
    additive_scrambler_bb_impl::work(int noutput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *)input_items[0];
      unsigned char *out = (unsigned char *)output_items[0];
      int reset_index = _get_next_reset_index(noutput_items);

      if (d_count >= 0) {
        for(int i = 0; i < noutput_items; i++) {
          unsigned char scramble_byte = 0x00;
          for (int k = 0; k < d_bits_per_byte; k++) {
            scramble_byte ^= (d_lfsr.next_bit() << k);
          }
          out[i] = in[i] ^ scramble_byte;
          d_bytes++;
          if (i == reset_index) {
            d_lfsr.reset();
            d_bytes = 0;
            reset_index = _get_next_reset_index(noutput_items, reset_index);
          }
        }
      } else {
        for(int i = 0; i < noutput_items; i++) {
          // Reset should occur at/before the item associated with the tag.
          if (i == reset_index) {
            d_lfsr.reset();
            d_bytes = 0;
            reset_index = _get_next_reset_index(noutput_items, reset_index);
          }
          unsigned char scramble_byte = 0x00;
          for (int k = 0; k < d_bits_per_byte; k++) {
            scramble_byte ^= (d_lfsr.next_bit() << k);
          }
          out[i] = in[i] ^ scramble_byte;
          d_bytes++;
        }
      }

      return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */

