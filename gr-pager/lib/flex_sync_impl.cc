/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2010,2012 Free Software Foundation, Inc.
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

#include "flex_sync_impl.h"
#include "flex_modes.h"
#include "bch3221.h"
#include "util.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/blocks/count_bits.h>
#include <cstdio>

namespace gr {
  namespace pager {

    flex_sync::sptr
    flex_sync::make()
    {
      return gnuradio::get_initial_sptr
	(new flex_sync_impl());
    }

    // FLEX sync block takes input from sliced baseband stream [0-3]
    // at specified channel rate. Symbol timing is established based
    // on receiving one of the defined FLEX protocol synchronization
    // words. The block outputs one FLEX frame worth of bits on each
    // output phase for the data portion of the frame. Unused phases
    // get all zeros, which are considered idle code words.

    flex_sync_impl::flex_sync_impl() :
      block("flex_sync",
	       io_signature::make(1, 1, sizeof(unsigned char)),
	       io_signature::make(4, 4, sizeof(unsigned char))),
      d_sync(10) // Fixed at 10 samples per baud (@ 1600 baud)
    {
      enter_idle();
    }

    flex_sync_impl::~flex_sync_impl()
    {
    }

    void
    flex_sync_impl::forecast(int noutput_items, gr_vector_int &inputs_required)
    {
      // samples per bit X number of outputs needed
      int items = noutput_items*d_spb;
      for(unsigned int i = 0; i < inputs_required.size(); i++)
        inputs_required[i] = items;
    }

    int
    flex_sync_impl::index_avg(int start, int end)
    {
      // modulo average
      if(start < end)
        return (end + start)/2;
      else
        return ((end + start)/2 + d_spb/2) % d_spb;
    }

    bool
    flex_sync_impl::test_sync(unsigned char sym)
    {
      // 64-bit FLEX sync code:
      // AAAA:BBBBBBBB:CCCC
      //
      // Where BBBBBBBB is always 0xA6C6AAAA
      // and AAAA^CCCC is 0xFFFF
      //
      // Specific values of AAAA determine what bps and encoding the
      // packet is beyond the frame information word
      //
      // First we match on the marker field with a hamming distance < 4
      // Then we match on the outer code with a hamming distance < 4

      d_sync[d_index] = (d_sync[d_index] << 1) | (sym < 2);
      int64_t val = d_sync[d_index];
      int32_t marker = ((val & 0x0000FFFFFFFF0000ULL)) >> 16;

      if(gr::blocks::count_bits32(marker^FLEX_SYNC_MARKER) < 4) {
        int32_t code = ((val & 0xFFFF000000000000ULL) >> 32) |
	  (val & 0x000000000000FFFFULL);

        for(int i = 0; i < num_flex_modes; i++) {
	  if(gr::blocks::count_bits32(code^flex_modes[i].sync) < 4) {
	    d_mode = i;
	    return true;
	  }
        }

        // Marker received but doesn't match known codes
        // All codes have high word inverted to low word
        unsigned short high = (code & 0xFFFF0000) >> 16;
        unsigned short low = code & 0x0000FFFF;
        unsigned short syn = high^low;
        if(syn == 0xFFFF)
	  fprintf(stderr, "Unknown sync code detected: %08X\n", code);
      }

      return false;
    }

    void
    flex_sync_impl::enter_idle()
    {
      d_state = ST_IDLE;
      d_index = 0;
      d_start = 0;
      d_center = 0;
      d_end = 0;
      d_count = 0;
      d_mode = 0;
      d_baudrate = 1600;
      d_levels = 2;
      d_spb = 16000/d_baudrate;
      d_bit_a = 0;
      d_bit_b = 0;
      d_bit_c = 0;
      d_bit_d = 0;
      d_hibit = false;
      fflush(stdout);
    }

    void
    flex_sync_impl::enter_syncing()
    {
      d_start = d_index;
      d_state = ST_SYNCING;
    }

    void
    flex_sync_impl::enter_sync1()
    {
      d_state = ST_SYNC1;
      d_end = d_index;
      d_center = index_avg(d_start, d_end); // Center of goodness
      d_count = 0;
    }

    void
    flex_sync_impl::enter_sync2()
    {
      d_state = ST_SYNC2;
      d_count = 0;
      d_baudrate = flex_modes[d_mode].baud;
      d_levels = flex_modes[d_mode].levels;
      d_spb = 16000/d_baudrate;

      if(d_baudrate == 3200) {
        // Oversampling buffer just got halved
        d_center = d_center/2;

	// We're here at the center of a 1600 baud bit
	// So this hack puts the index and bit counter
	// in the right place for 3200 bps.
        d_index = d_index/2-d_spb/2;
	d_count = -1;
      }
    }

    void
    flex_sync_impl::enter_data()
    {
      d_state = ST_DATA;
      d_count = 0;
    }

    void
    flex_sync_impl::parse_fiw()
    {
      // Nothing is done with these now, but these will end up getting
      // passed as metadata when mblocks are available

      // Bits 31-28 are frame number related, but unknown function
      // This might be a checksum
      d_unknown2 = reverse_bits8((d_fiw >> 24) & 0xF0);

      // Cycle is bits 27-24, reversed
      d_cycle = reverse_bits8((d_fiw >> 20) & 0xF0);

      // Frame is bits 23-17, reversed
      d_frame = reverse_bits8((d_fiw >> 16) & 0xFE);

      // Bits 16-11 are some sort of marker, usually identical across
      // many frames but sometimes changes between frames or modes
      d_unknown1 = (d_fiw >> 11) & 0x3F;

      //printf("CYC:%02i FRM:%03i\n", d_cycle, d_frame);
    }

    int
    flex_sync_impl::output_symbol(unsigned char sym)
    {
      // Here is where we output a 1 or 0 on each phase according
      // to current FLEX mode and symbol value.  Unassigned phases
      // are zero from the enter_idle() initialization.
      //
      // FLEX can transmit the data portion of the frame at either
      // 1600 bps or 3200 bps, and can use either two- or four-level
      // FSK encoding.
      //
      // At 1600 bps, 2-level, a single "phase" is transmitted with bit
      // value '0' using level '3' and bit value '1' using level '0'.
      //
      // At 1600 bps, 4-level, a second "phase" is transmitted, and the
      // di-bits are encoded with a gray code:
      //
      // Symbol	Phase 1  Phase 2
      // ------   -------  -------
      //   0         1        1
      //   1         1        0
      //   2         0        0
      //   3         0        1
      //
      // At 1600 bps, 4-level, these are called PHASE A and PHASE B.
      //
      // At 3200 bps, the same 1 or 2 bit encoding occurs, except that
      // additionally two streams are interleaved on alternating symbols.
      // Thus, PHASE A (and PHASE B if 4-level) are decoded on one symbol,
      // then PHASE C (and PHASE D if 4-level) are decoded on the next.

      int bits = 0;

      if(d_baudrate == 1600) {
	d_bit_a = (sym < 2);
	if(d_levels == 4)
	  d_bit_b = (sym == 0) || (sym == 3);

	*d_phase_a++ = d_bit_a;
	*d_phase_b++ = d_bit_b;
	*d_phase_c++ = d_bit_c;
	*d_phase_d++ = d_bit_d;
	bits++;
      }
      else {
	if(!d_hibit) {
	  d_bit_a = (sym < 2);
	  if(d_levels == 4)
	    d_bit_b = (sym == 0) || (sym == 3);
	  d_hibit = true;
	}
	else {
	  d_bit_c = (sym < 2);
	  if(d_levels == 4)
	    d_bit_d = (sym == 0) || (sym == 3);
	  d_hibit = false;

	  *d_phase_a++ = d_bit_a;
	  *d_phase_b++ = d_bit_b;
	  *d_phase_c++ = d_bit_c;
	  *d_phase_d++ = d_bit_d;
	  bits++;
	}
      }

      return bits;
    }

    int
    flex_sync_impl::general_work(int noutput_items,
				 gr_vector_int &ninput_items,
				 gr_vector_const_void_star &input_items,
				 gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *)input_items[0];
      d_phase_a = (unsigned char *)output_items[0];
      d_phase_b = (unsigned char *)output_items[1];
      d_phase_c = (unsigned char *)output_items[2];
      d_phase_d = (unsigned char *)output_items[3];

      int i = 0, j = 0;
      int ninputs = ninput_items[0];

      while(i < ninputs && j < noutput_items) {
        unsigned char sym = *in++; i++;
	d_index = (d_index+1) % d_spb;

        switch(d_state) {
	case ST_IDLE:
	  // Continually compare the received symbol stream
	  // against the known FLEX sync words.
	  if(test_sync(sym))
	    enter_syncing();
	  break;

	case ST_SYNCING:
	  // Wait until we stop seeing sync, then calculate
	  // the center of the bit period (d_center)
	  if(!test_sync(sym))
	    enter_sync1();
	  break;

	case ST_SYNC1:
	  // Skip 16 bits of dotting, then accumulate 32 bits
	  // of Frame Information Word.
	  if(d_index == d_center) {
	    d_fiw = (d_fiw << 1) | (sym > 1);
	    if(++d_count == 48) {
	      // FIW is accumulated, call BCH to error correct it
	      bch3221(d_fiw);
	      parse_fiw();
	      enter_sync2();
	    }
	  }
	  break;

	case ST_SYNC2:
	  // This part and the remainder of the frame are transmitted
	  // at either 1600 bps or 3200 bps based on the received
	  // FLEX sync word. The second SYNC header is 25ms of idle bits
	  // at either speed.
	  if(d_index == d_center) {
	    // Skip 25 ms = 40 bits @ 1600 bps, 80 @ 3200 bps
	    if(++d_count == d_baudrate/40)
	      enter_data();
	  }
	  break;

	case ST_DATA:
	  // The data portion of the frame is 1760 ms long at either
	  // baudrate. This is 2816 bits @ 1600 bps and 5632 bits @ 3200 bps.
	  // The output_symbol() routine decodes and doles out the bits
	  // to each of the four transmitted phases of FLEX interleaved codes.
	  if(d_index == d_center) {
	    j += output_symbol(sym);
	    if(++d_count == d_baudrate*1760/1000)
	      enter_idle();
	  }
	  break;

	default:
	  assert(0); // memory corruption of d_state if ever gets here
	  break;
        }
      }

      consume_each(i);
      return j;
    }

  } /* namespace pager */
} /* namespace gr */
