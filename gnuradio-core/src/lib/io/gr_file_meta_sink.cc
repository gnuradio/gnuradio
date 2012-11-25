/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gr_file_meta_sink.h>
#include <gr_io_signature.h>
#include <stdexcept>

#define HEADER_SIZE 117

gr_file_meta_sink_sptr
gr_make_file_meta_sink(size_t itemsize, const char *filename,
		       double samp_rate, double relative_rate,
		       gr_file_types type, bool complex,
		       const std::string &extra_dict)
{
  return gnuradio::get_initial_sptr
    (new gr_file_meta_sink(itemsize, filename,
			   samp_rate, relative_rate,
			   type, complex,
			   extra_dict));
}

gr_file_meta_sink::gr_file_meta_sink(size_t itemsize, const char *filename,
				     double samp_rate, double relative_rate,
				     gr_file_types type, bool complex,
				     const std::string &extra_dict)
  : gr_sync_block("file_meta_sink",
		  gr_make_io_signature(1, 1, itemsize),
		  gr_make_io_signature(0, 0, 0)),
    gr_file_sink_base(filename, true),
    d_itemsize(itemsize), d_relative_rate(relative_rate),
    d_total_seg_size(0)
{
  if(!open(filename))
    throw std::runtime_error("file_meta_sink: can't open file\n");

  pmt_t timestamp = pmt_make_tuple(pmt_from_uint64(0),
				   pmt_from_double(0));

  d_header = pmt_make_dict();
  d_header = pmt_dict_add(d_header, mp("rx_rate"), mp(samp_rate));
  d_header = pmt_dict_add(d_header, mp("rx_time"), timestamp);
  d_header = pmt_dict_add(d_header, mp("type"), pmt_from_long(type));
  d_header = pmt_dict_add(d_header, mp("cplx"), complex ? PMT_T : PMT_F);
  d_header = pmt_dict_add(d_header, mp("strt"), pmt_from_uint64(HEADER_SIZE));
  d_header = pmt_dict_add(d_header, mp("size"), pmt_from_uint64(0));
  // handle extra dictionary

  write_header(d_header);
}

void
gr_file_meta_sink::write_header(pmt_t header)
{
  do_update();

  std::string header_str = pmt_serialize_str(header);
  if(header_str.size() != HEADER_SIZE)
      throw std::runtime_error("file_meta_sink: header is wrong size.\n");

  size_t nwritten = 0;
  while(nwritten < header_str.size()) {
    std::string sub = header_str.substr(nwritten);
    int count = fwrite(sub.c_str(), sizeof(char), sub.size(), d_fp);
    nwritten += count;
    if((count == 0) && (ferror(d_fp))) {
      fclose(d_fp);
      throw std::runtime_error("file_meta_sink: error writing header to file.\n");
    }
  }
}

bool
gr_file_meta_sink::update_header(pmt_t key, pmt_t value)
{
  // Special handling caveat to transform rate from radio source into
  // the rate at this sink.
  if(pmt_eq(key, mp("rx_rate"))) {
    double rate = pmt_to_double(value);
    value = pmt_from_double(rate*d_relative_rate);
  }

  if(pmt_dict_has_key(d_header, key)) {
    d_header = pmt_dict_add(d_header, key, value);
    return true;
  }
  else {
    return false;
  }
}

uint64_t
gr_file_meta_sink::get_last_header_loc()
{
  uint64_t loc = 0;
  pmt_t v = pmt_dict_ref(d_header, mp("strt"), PMT_NIL);
  if(!pmt_eq(v, PMT_NIL))
    loc = pmt_to_uint64(v) - HEADER_SIZE;
  return loc;
}

gr_file_meta_sink::~gr_file_meta_sink()
{
  // Replace the last header block with the final count of the number
  // of items.
  uint64_t loc = get_last_header_loc();
  uint64_t seg_size = nitems_read(0) * d_itemsize - d_total_seg_size;
  pmt_t s = pmt_from_uint64(seg_size);
  //std::cerr << "Destructor" << std::endl;
  //std::cerr << "  location of last header: " << loc << std::endl;
  //std::cerr << "  nitems_read:  " << nitems_read(0)*d_itemsize << std::endl;
  //std::cerr << "  Segment Size: " << seg_size << std::endl;
  if(update_header(mp("size"), s)) {
    fseek(d_fp, loc, SEEK_SET);
    write_header(d_header);
  }
}

int
gr_file_meta_sink::work(int noutput_items,
			gr_vector_const_void_star &input_items,
			gr_vector_void_star &output_items)
{
  char *inbuf = (char*)input_items[0];
  int  nwritten = 0;

  do_update();				// update d_fp is reqd

  if(!d_fp)
    return noutput_items;		// drop output on the floor

  uint64_t abs_N = nitems_read(0);
  uint64_t end_N = abs_N + (uint64_t)(noutput_items);
  std::vector<gr_tag_t> all_tags;
  get_tags_in_range(all_tags, 0, abs_N, end_N);
  
  std::vector<gr_tag_t>::iterator itr;
  for(itr = all_tags.begin(); itr != all_tags.end(); itr++) {
    // Special case where info is carried on the first tag, so we just
    // overwrite the first header.
    if(itr->offset == 0) {
      if(update_header(itr->key, itr->value)) {
	fseek(d_fp, 0, SEEK_SET);
	write_header(d_header);
      }
    }
    else {
      // Update the last header info with the number of samples this
      // block represents.
      uint64_t loc = get_last_header_loc();
      uint64_t seg_size = itr->offset * d_itemsize - d_total_seg_size;
      pmt_t s = pmt_from_uint64(seg_size);
      //std::cerr << "Found Tag at: " << itr->offset*d_itemsize << std::endl;
      //std::cerr << "  last header starts at: " << loc << std::endl;
      //std::cerr << "  segment size is:       " << seg_size << std::endl;    
      if(update_header(mp("size"), s)) {
	fseek(d_fp, loc, SEEK_SET);
	write_header(d_header);
      }

      if(update_header(itr->key, itr->value)) {
	// Otherwise, set current size of chunk to 0 and start of
	// chunk based on current index + header size.
	d_total_seg_size += seg_size;
	s = pmt_from_uint64(0);
	if(update_header(mp("size"), s)) {
	  // If we have multiple tags on the same offset, this makes
	  // sure we just overwrite the same header each time instead
	  // of creating a new header per tag.
	  uint64_t seg_start = loc;
	  if(seg_size != 0)
	    seg_start += HEADER_SIZE + seg_size;
	  pmt_t s = pmt_from_uint64(seg_start + HEADER_SIZE);
	  if(update_header(mp("strt"), s)) {
	    //std::cerr << "Adding new header" << std::endl;
	    //std::cerr << "  new header start at: " << seg_start-HEADER_SIZE << std::endl;
	    //std::cerr << "  new seg start at:    " << seg_start << std::endl;
	    fseek(d_fp, seg_start, SEEK_SET);
	    write_header(d_header);
	  }
	}
      }
    }
  }

  while(nwritten < noutput_items) {
    int count = fwrite(inbuf, d_itemsize, noutput_items - nwritten, d_fp);
    if(count == 0)	// FIXME add error handling
      break;
    nwritten += count;
    inbuf += count * d_itemsize;
  }

  if(d_unbuffered)
    fflush(d_fp);

  return nwritten;
}
