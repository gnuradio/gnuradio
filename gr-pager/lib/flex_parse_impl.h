/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PAGER_FLEX_PARSE_IMPL_H
#define INCLUDED_PAGER_FLEX_PARSE_IMPL_H

#include <gnuradio/pager/flex_parse.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>
#include "flex_modes.h"
#include <sstream>
#include <map>
#include <utility>

namespace gr {
  namespace pager {

#define FIELD_DELIM ((unsigned char)128)

    class flex_parse_impl : public flex_parse
    {
    private:
      msg_queue::sptr d_queue;
      float d_freq;
      std::vector<uint32_t> d_datawords;
      std::ostringstream d_payload;
      std::map<int, std::ostringstream*> d_split_queue;

      void parse_frame(std::vector<uint32_t> &data);

    public:
      flex_parse_impl(msg_queue::sptr queue, float freq);
      ~flex_parse_impl() { }

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

    bool is_active_codeword(uint32_t word);

    class flex_frame;
    class flex_header;
    class flex_page;
    class flex_page_iter;

    class flex_frame
    {
      private:
	std::vector<uint32_t> &d_datawords;

	static bool is_active_codeword(uint32_t word);

	/* FLEX data frames (that is, 88 data words per phase recovered
	   after sync, symbol decoding, dephasing, deinterleaving, error
	   correction, and conversion from codewords to data words) start
	   with a block information word containing indices of the page
	   address field and page vector fields.  */

	uint32_t get_block_info();
	size_t get_vector_offset();
	size_t get_address_offset();

      public:
	flex_frame(std::vector<uint32_t> &datawords): d_datawords(datawords) { }
	virtual ~flex_frame() { }

	uint32_t get_word(size_t i);
	bool is_active();
	bool validate();
	bool validate(size_t start, size_t len);
	bool parse(std::ostringstream &stream);
	flex_page_iter begin();
	flex_page_iter end();
    };

    class flex_header
    {
      private:
	uint32_t get_vector_field();
	virtual uint32_t get_meta() = 0;
	virtual int get_capcode() = 0;

      protected:
	flex_frame *d_frame;
	unsigned int d_page_id;
	size_t d_aoffset;
	size_t d_voffset;

	page_type_t get_type();
	size_t get_message_start();
	size_t get_message_length();
	bool validate();

      public:
	flex_header(flex_frame *frame, unsigned int page_id, size_t aoffset,
	    size_t voffset) :
	  d_frame(frame), d_page_id(page_id), d_aoffset(aoffset),
	  d_voffset(voffset) { }
	virtual ~flex_header() { }

	bool parse(std::ostringstream &stream);
	virtual flex_page *generate_page() = 0;
    };

    class flex_header_inactive : public flex_header
    {
      private:
	uint32_t get_meta();
	int get_capcode();

      public:
	flex_header_inactive() : flex_header(NULL, 0, 0, 0) { }
	virtual ~flex_header_inactive() { }
	flex_page *generate_page();
    };

    class flex_header_laddr : public flex_header
    {
      private:
	uint32_t get_meta();
	int get_capcode();

      public:
	flex_header_laddr(flex_frame *frame, unsigned int page_id, size_t aoffset,
	    size_t voffset) : flex_header(frame, page_id, aoffset, voffset) { }
	virtual ~flex_header_laddr() { }

	flex_page *generate_page();
    };

    class flex_header_saddr : public flex_header
    {
      private:
	uint32_t get_meta();
	int get_capcode();

      public:
	flex_header_saddr(flex_frame *frame, unsigned int page_id, size_t aoffset,
	    size_t voffset) : flex_header(frame, page_id, aoffset, voffset) { }
	virtual ~flex_header_saddr() {}

	flex_page *generate_page();
    };

    class flex_page
    {
      protected:
	flex_frame *d_frame;
	flex_header *d_header;
	uint32_t d_meta;
	size_t d_mw1;
	size_t d_len;

	bool is_valid_alphanumeric(unsigned char c);

      public:
	flex_page(flex_frame *frame, flex_header *header, uint32_t meta,
	    size_t mw1, size_t len)
	  : d_frame(frame), d_header(header), d_meta(meta), d_mw1(mw1),
	    d_len(len) { };
	virtual ~flex_page() { };

        virtual bool parse(std::ostringstream &stream,
	    std::map<int, std::ostringstream*> &split_queue);
    };

    class flex_page_inactive : public flex_page
    {
      public:
	flex_page_inactive() : flex_page(NULL, NULL, 0, 0, 0) { }
	virtual ~flex_page_inactive() { }

	bool parse(std::ostringstream &stream,
	    std::map<int, std::ostringstream*> &split_queue);
    };

    class flex_page_aln : public flex_page
    {
      private:
	int d_capcode;
        std::map<int, std::ostringstream*> d_split_queue;

	bool parse_page(std::ostringstream &stream);

      public:
	flex_page_aln(flex_frame *frame, flex_header *header, uint32_t meta,
	    size_t mw1, size_t len, int capcode) :
	  flex_page(frame, header, ((meta >> 11) & 0x03), mw1, (len - 1)),
	  d_capcode(capcode) { }
	virtual ~flex_page_aln() { }

	bool parse(std::ostringstream &stream,
	    std::map<int, std::ostringstream*> &split_queue);
    };

    class flex_page_num : public flex_page
    {
      private:
	int d_type;

      public:
	flex_page_num(flex_frame *frame, flex_header *header, uint32_t meta,
	    size_t mw1, size_t len, size_t type) :
	  flex_page(frame, header, meta, mw1, len), d_type(type & 0x7) { }
	virtual ~flex_page_num() { }

	bool parse(std::ostringstream &stream,
	    std::map<int, std::ostringstream*> &split_queue);
    };

    class flex_page_iter
    {
      private:
	unsigned int d_i;
	flex_frame &d_frame;
	size_t d_aoffset;
	size_t d_voffset;
	unsigned int d_max;
	flex_header *d_header;
	flex_page *d_page;

	bool is_active_codeword(uint32_t word);
	bool is_active();
	bool has_laddr();
	flex_header *generate_header();
	void next();
	flex_page *get_page();

      public:
	flex_page_iter(flex_frame &frame, size_t aoffset, size_t voffset)
	  : d_frame(frame), d_aoffset(aoffset), d_voffset(voffset),
	    d_max(voffset - aoffset)
	{
	  d_i = 0;
	  d_header = NULL;
	  d_page = NULL;
	};

	virtual ~flex_page_iter() { 
	  delete d_header;
	  delete d_page;
	};

	flex_page_iter operator++(int junk);
	flex_page* operator->();
	bool operator==(const flex_page_iter& rhs);
	bool operator!=(const flex_page_iter& rhs);
    };

  } /* namespace pager */
} /* namespace gr */

#endif /* INCLUDED_PAGER_FLEX_PARSE_IMPL_H */
