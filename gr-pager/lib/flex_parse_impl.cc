/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2007,2010,2012 Free Software Foundation, Inc.
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

#include "flex_parse_impl.h"
#include "bch3221.h"
#include <gnuradio/io_signature.h>
#include <ctype.h>
#include <iostream>
#include <iomanip>

namespace gr {
  namespace pager {

    flex_parse::sptr
    flex_parse::make(msg_queue::sptr queue, float freq)
    {
      return gnuradio::get_initial_sptr
	(new flex_parse_impl(queue, freq));
    }

    flex_parse_impl::flex_parse_impl(msg_queue::sptr queue, float freq) :
      sync_block("flex_parse",
		    io_signature::make(1, 1, sizeof(int32_t)),
		    io_signature::make(0, 0, 0)),
      d_queue(queue),
      d_freq(freq)
    {
      d_count = 0;
    }

    flex_parse_impl::~flex_parse_impl()
    {
    }

    int flex_parse_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      const int32_t *in = (const int32_t *)input_items[0];

      int i = 0;
      while(i < noutput_items) {
	// Accumulate one whole frame's worth of data words (88 of them)
	d_datawords[d_count] = *in++; i++;
	if(++d_count == 88) {
	  parse_data();
	  d_count = 0;
	}
      }

      return i;
    }

    /* FLEX data frames (that is, 88 data words per phase recovered
       after sync, symbol decoding, dephasing, deinterleaving, error
       correction, and conversion from codewords to data words) start
       with a block information word containing indices of the page
       address field and page vector fields.  */
    void
    flex_parse_impl::parse_capcode(int aw1, int aw2)
    {
      d_laddr = (aw1 < 0x008001L) ||
	(aw1 > 0x1E0000L) ||
	(aw1 > 0x1E7FFEL);

      if(d_laddr)
        d_capcode = aw1+((aw2^0x001FFFFF)<<15)+0x1F9000;  // Don't ask
      else
        d_capcode = aw1-0x8000;
    }

    void
    flex_parse_impl::parse_data()
    {
      // Block information word is the first data word in frame
      int biw = d_datawords[0];

      // Nothing to see here, please move along
      if(biw == 0 || biw == 0x001FFFFF)
	return;

      // Vector start index is bits 15-10
      // Address start address is bits 9-8, plus one for offset
      int voffset = (biw >> 10) & 0x3f;
      int aoffset = ((biw >> 8) & 0x03) + 1;

      //printf("BIW:%08X AW:%02i-%02i\n", biw, aoffset, voffset);

      // Iterate through pages and dispatch to appropriate handler
      for(int i = aoffset; i < voffset; i++) {
	int j = voffset+i-aoffset;		// Start of vector field for address @ i

	if(d_datawords[i] == 0x00000000 ||
	   d_datawords[i] == 0x001FFFFF)
	  continue;				// Idle codewords, invalid address

	parse_capcode(d_datawords[i], d_datawords[i+1]);
        if(d_laddr)
	  i++;

        if(d_capcode < 0)			// Invalid address, skip
          continue;

        // Parse vector information word for address @ offset 'i'
	int viw = d_datawords[j];
	d_type = (page_type_t)((viw >> 4) & 0x00000007);
	int mw1 = (viw >> 7) & 0x00000007F;
	int len = (viw >> 14) & 0x0000007F;

	if(is_numeric_page(d_type))
	  len &= 0x07;
        int mw2 = mw1+len;

	if(mw1 == 0 && mw2 == 0)
	  continue;				// Invalid VIW

	if(is_tone_page(d_type))
	  mw1 = mw2 = 0;

	if(mw1 > 87 || mw2 > 87)
	  continue;				// Invalid offsets

	d_payload.str("");
	d_payload.setf(std::ios::showpoint);
	d_payload << std::setprecision(6) << std::setw(7)
		  << d_freq/1e6 << FIELD_DELIM
		  << std::setw(10) << d_capcode << FIELD_DELIM
		  << flex_page_desc[d_type] << FIELD_DELIM;

	if(is_alphanumeric_page(d_type))
	  parse_alphanumeric(mw1, mw2-1, j);
	else if(is_numeric_page(d_type))
	  parse_numeric(mw1, mw2, j);
	else if(is_tone_page(d_type))
	  parse_tone_only();
	else
	  parse_unknown(mw1, mw2);

	message::sptr msg = message::make_from_string(std::string(d_payload.str()));
	d_queue->handle(msg);
      }
    }

    void
    flex_parse_impl::parse_alphanumeric(int mw1, int mw2, int j)
    {
      int frag;
      //bool cont;

      if(!d_laddr) {
	frag = (d_datawords[mw1] >> 11) & 0x03;
	//cont = (d_datawords[mw1] >> 10) & 0x01;
	mw1++;
      }
      else {
	frag = (d_datawords[j+1] >> 11) & 0x03;
	//cont = (d_datawords[j+1] >> 10) & 0x01;
	mw2--;
      }

      //d_payload << frag << FIELD_DELIM;
      //d_payload << cont << FIELD_DELIM;

      for(int i = mw1; i <= mw2; i++) {
	int dw = d_datawords[i];
	unsigned char ch;

	if(i > mw1 || frag != 0x03) {
	  ch = dw & 0x7F;
	  if(ch != 0x03)
	    d_payload << ch;
	}

	ch = (dw >> 7) & 0x7F;
	if(ch != 0x03)	// Fill
	  d_payload << ch;

	ch = (dw >> 14) & 0x7F;
	if(ch != 0x03)	// Fill
	  d_payload << ch;
      }
    }

    void
    flex_parse_impl::parse_numeric(int mw1, int mw2, int j)
    {
      // Get first dataword from message field or from second
      // vector word if long address
      int dw;
      if(!d_laddr) {
	dw = d_datawords[mw1];
	mw1++;
	mw2++;
      }
      else {
	dw = d_datawords[j+1];
      }

      unsigned char digit = 0;
      int count = 4;
      if(d_type == FLEX_NUMBERED_NUMERIC)
	count += 10;	// Skip 10 header bits for numbered numeric pages
      else
	count += 2;	// Otherwise skip 2

      for(int i = mw1; i <= mw2; i++) {
	for(int k = 0; k < 21; k++) {
	  // Shift LSB from data word into digit
	  digit = (digit >> 1) & 0x0F;
	  if(dw & 0x01)
	    digit ^= 0x08;
	  dw >>= 1;
	  if(--count == 0) {
	    if(digit != 0x0C) // Fill
	      d_payload << flex_bcd[digit];
	    count = 4;
	  }
	}

	dw = d_datawords[i];
      }
    }

    void
    flex_parse_impl::parse_tone_only()
    {
    }

    void
    flex_parse_impl::parse_unknown(int mw1, int mw2)
    {
    }

  } /* namespace pager */
} /* namespace gr */
