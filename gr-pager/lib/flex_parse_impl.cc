/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2007,2010,2012,2016 Free Software Foundation, Inc.
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
#include <gnuradio/io_signature.h>
#include <ctype.h>
#include <iostream>
#include <iomanip>

#define FLEX_IDLE_CODEWORD 0x001FFFFF

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
		    io_signature::make(1, 1, sizeof(uint32_t)),
		    io_signature::make(0, 0, 0)),
      d_queue(queue),
      d_freq(freq)
    {
    }

    int flex_parse_impl::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
    {
      const uint32_t *in = (const uint32_t *)input_items[0];

      int i = 0;
      while(i < noutput_items) {
	// Accumulate one whole frame's worth of data words (88 of them)
	d_datawords.push_back(*in++);
	i++;
	if(d_datawords.size() == 88) {
	  parse_frame(d_datawords);
	  d_datawords.clear();
	}
      }

      return i;
    }

    void
    flex_parse_impl::parse_frame(std::vector<uint32_t> &data)
    {
      flex_frame frame(data);

      if (!frame.is_active())
	return;

      if (!frame.validate())
	return;

      for (flex_page_iter i = frame.begin(); i != frame.end(); i++) {

	d_payload.str("");
	d_payload.setf(std::ios::showpoint);
	d_payload << std::setprecision(6) << std::setw(7)
	  << d_freq/1e6 << FIELD_DELIM;

	if (i->parse(d_payload))
	  d_queue->handle(message::make_from_string(d_payload.str()));
      }
    }

    bool
    is_active_codeword(uint32_t word)
    {
      return !(word == 0 || word == FLEX_IDLE_CODEWORD);
    }

    bool
    flex_page_iter::is_active()
    {
      return gr::pager::is_active_codeword(d_frame.get_word(d_aoffset + d_i));
    }

    bool
    flex_page_iter::has_laddr()
    {
      int aw1 = d_frame.get_word(d_aoffset + d_i);

      return (aw1 < 0x008001L) || (aw1 > 0x1E0000L) || (aw1 > 0x1E7FFEL);
    }

    flex_header *
    flex_page_iter::generate_header()
    {
      if (!is_active())
	return new flex_header_inactive();

      if (has_laddr())
	return new flex_header_laddr(&d_frame, d_i, d_aoffset, d_voffset);

      return new flex_header_saddr(&d_frame, d_i, d_aoffset, d_voffset);
    }

    void
    flex_page_iter::next()
    {
      delete d_header;
      d_header = NULL;
      delete d_page;
      d_page = NULL;

      if (is_active())
	if (has_laddr())
	  d_i += 2;
	else
	  d_i += 1;
      else
	d_i = d_max;

      if (d_i > d_max)
	d_i = d_max;
    }

    flex_page *
    flex_page_iter::get_page()
    {
      d_header = generate_header();
      d_page = d_header->generate_page();

      return d_page;
    }

    flex_page_iter
    flex_page_iter::operator++(int junk)
    {
      next();
      return *this;
    }

    flex_page*
    flex_page_iter::operator->()
    {
      return get_page();
    }

    bool
    flex_page_iter::operator==(const flex_page_iter& rhs)
    {
      return &d_frame == &(rhs.d_frame) &&
	(d_aoffset + d_i) == (rhs.d_aoffset + rhs.d_i);
    }

    bool
    flex_page_iter::operator!=(const flex_page_iter &rhs)
    {
      return !(*this == rhs);
    }

    uint32_t
    flex_frame::get_block_info()
    {
      // Block information word is the first data word in frame
      return d_datawords[0];
    }

    size_t
    flex_frame::get_vector_offset()
    {
      // Vector start index is bits 15-10
      return (get_block_info() >> 10) & 0x3f;
    }

    size_t
    flex_frame::get_address_offset()
    {
      // Address start address is bits 9-8, plus one for offset
      return ((get_block_info() >> 8) & 0x03) + 1;
    }

    uint32_t
    flex_frame::get_word(size_t i)
    {
      return d_datawords.at(i);
    }

    bool
    flex_frame::is_active()
    {
      return gr::pager::is_active_codeword(get_block_info());
    }

    bool
    flex_frame::parse(std::ostringstream &stream)
    {
      return true;
    }

    bool
    flex_frame::validate()
    {
      size_t voffset = get_vector_offset();
      size_t aoffset = get_address_offset();

      return (2 * voffset - aoffset) < d_datawords.size();
    }

    bool
    flex_frame::validate(size_t start, size_t len)
    {
      return start > get_vector_offset() &&
	(start + len) < d_datawords.size();
    }

    flex_page_iter
    flex_frame::begin()
    {
      int aoffset = get_address_offset();
      int voffset = get_vector_offset();

      if (aoffset > voffset)
	aoffset = voffset;

      return flex_page_iter(*this, aoffset, voffset);
    }

    flex_page_iter
    flex_frame::end()
    {
      return flex_page_iter(*this, get_vector_offset(), get_vector_offset());
    }

    uint32_t
    flex_header::get_vector_field()
    {
      return d_frame->get_word(d_voffset + d_page_id);
    }

    page_type_t
    flex_header::get_type()
    {
      return (page_type_t)((get_vector_field() >> 4) & 0x7);
    }

    size_t
    flex_header::get_message_start()
    {
      return (get_vector_field() >> 7) & 0x00000007F;
    }

    size_t
    flex_header::get_message_length()
    {
      return (get_vector_field() >> 14) & 0x0000007F;
    }

    bool
    flex_header::validate()
    {
      size_t start = get_message_start();

      if (!d_frame->validate(start, get_message_length()))
	return false;

      return get_capcode() >= 0 &&
       start > (d_voffset + d_page_id);
    }

    bool
    flex_header::parse(std::ostringstream &stream)
    {
      if (d_frame->parse(stream)) {
	stream << std::setw(10) << get_capcode() << FIELD_DELIM
	  << flex_page_desc[get_type()] << FIELD_DELIM;
	return true;
      }

      return false;
    }

    uint32_t
    flex_header_inactive::get_meta()
    {
      return 0;
    }

    int
    flex_header_inactive::get_capcode()
    {
      return -1;
    }

    flex_page *
    flex_header_inactive::generate_page()
    {
      return new flex_page_inactive();
    }

    uint32_t
    flex_header_laddr::get_meta()
    {
      return d_frame->get_word(d_voffset + d_page_id + 1);
    }

    int
    flex_header_laddr::get_capcode()
    {
      int aw1 = d_frame->get_word(d_aoffset + d_page_id);
      int aw2 = d_frame->get_word(d_aoffset + d_page_id + 1);

      return aw1 + ((aw2 ^ FLEX_IDLE_CODEWORD) << 15) + 0x1F9000;
    }

    flex_page *
    flex_header_laddr::generate_page()
    {
      page_type_t type;
      size_t mw1;
      size_t len;

      if (!validate())
	return new flex_page_inactive();

      type = get_type();
      mw1 = get_message_start();
      len = get_message_length();

      if (is_alphanumeric_page(type)) {
	return new flex_page_aln(d_frame, this, get_meta(), mw1, (len - 1),
	    get_capcode());
      } else if (is_numeric_page(type)) {
	return new flex_page_num(d_frame, this, get_meta(), (mw1 + 1), len, type);
      }

      return new flex_page_inactive();
    }

    int
    flex_header_saddr::get_capcode()
    {
      return d_frame->get_word(d_aoffset + d_page_id) - 0x8000;
    }

    uint32_t
    flex_header_saddr::get_meta()
    {
      return d_frame->get_word(get_message_start());
    }

    flex_page *
    flex_header_saddr::generate_page()
    {
      page_type_t type;
      size_t mw1;
      size_t len;

      if (!validate())
	return new flex_page_inactive();

      type = get_type();
      mw1 = get_message_start() + 1;
      len = get_message_length();

      if (is_alphanumeric_page(type)) {
	return new flex_page_aln(d_frame, this, get_meta(), mw1, len - 1,
	    get_capcode());
      } else if (is_numeric_page(type)) {
	return new flex_page_num(d_frame, this, get_meta(), mw1, len, type);
      }

      return new flex_page_inactive();
    }

    bool
    flex_page::is_valid_alphanumeric(unsigned char c)
    {
      return c != 0x03;
    }

    bool
    flex_page::parse(std::ostringstream &stream)
    {
      return d_header->parse(stream);
    }

    bool
    flex_page_inactive::parse(std::ostringstream &stream)
    {
      return false;
    }

    bool
    flex_page_aln::parse_page(std::ostringstream &stream)
    {
      for(size_t i = d_mw1; i <= (d_mw1 + d_len); i++) {
	int dw = d_frame->get_word(i);
	unsigned char ch;

	if(i > d_mw1 || d_meta != 0x03) {
	  ch = dw & 0x7F;
	  if(is_valid_alphanumeric(ch))
	    stream << ch;
	}

	ch = (dw >> 7) & 0x7F;
	if(is_valid_alphanumeric(ch))
	  stream << ch;

	ch = (dw >> 14) & 0x7F;
	if(is_valid_alphanumeric(ch))
	  stream << ch;
      }

      return true;
    }

    bool
    flex_page_aln::parse(std::ostringstream &stream)
    {
      int mw2 = d_mw1 + d_len;

      if (d_mw1 > 87 || mw2 > 87)
	return false;

      if (!flex_page::parse(stream))
	return false;

      return parse_page(stream);
    }

    bool
    flex_page_num::parse(std::ostringstream &stream)
    {
      int dw = d_meta;
      unsigned char digit = 0;
      int count = 4;

      if (!flex_page::parse(stream))
	return false;

      // Skip 10 header bits for numbered numeric pages, otherwise 2
      count += (d_type == FLEX_NUMBERED_NUMERIC) ? 10 : 2;

      for(size_t i = d_mw1; i <= (d_mw1 + d_len); i++) {
	for(int k = 0; k < 21; k++) {
	  // Shift LSB from data word into digit
	  digit = (digit >> 1) & 0x0F;
	  if(dw & 0x01)
	    digit ^= 0x08;
	  dw >>= 1;
	  if(--count == 0) {
	    if(digit != 0x0C)
	      stream << flex_bcd[digit];
	    count = 4;
	  }
	}

	dw = d_frame->get_word(i);
      }

      return true;
    }
  } /* namespace pager */
} /* namespace gr */
