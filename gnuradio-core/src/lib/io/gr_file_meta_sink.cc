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

gr_file_meta_sink_sptr
gr_make_file_meta_sink(size_t itemsize, const char *filename,
		       double samp_rate, double relative_rate,
		       gr_file_types type, bool complex,
		       size_t max_segment_size,
		       const std::string &extra_dict)
{
  return gnuradio::get_initial_sptr
    (new gr_file_meta_sink(itemsize, filename,
			   samp_rate, relative_rate,
			   type, complex,
			   max_segment_size,
			   extra_dict));
}

gr_file_meta_sink::gr_file_meta_sink(size_t itemsize, const char *filename,
				     double samp_rate, double relative_rate,
				     gr_file_types type, bool complex,
				     size_t max_segment_size,
				     const std::string &extra_dict)
  : gr_sync_block("file_meta_sink",
		  gr_make_io_signature(1, 1, itemsize),
		  gr_make_io_signature(0, 0, 0)),
    gr_file_sink_base(filename, true),
    d_itemsize(itemsize), d_relative_rate(relative_rate),
    d_max_seg_size(max_segment_size), d_total_seg_size(0)
{
  if(!open(filename))
    throw std::runtime_error("file_meta_sink: can't open file\n");

  pmt_t timestamp = pmt_make_tuple(pmt_from_uint64(0),
				   pmt_from_double(0));

  // handle extra dictionary
  d_extra = pmt_make_dict();
  if(extra_dict.size() > 0) {
    pmt_t extras = pmt_deserialize_str(extra_dict);
    pmt_t keys = pmt_dict_keys(extras);
    pmt_t vals = pmt_dict_values(extras);
    size_t nitems = pmt_length(keys);
    for(size_t i = 0; i < nitems; i++) {
      d_extra = pmt_dict_add(d_extra,
			     pmt_nth(i, keys),
			     pmt_nth(i, vals));
    }
  }

  d_extra_size = pmt_serialize_str(d_extra).size();

  d_header = pmt_make_dict();
  d_header = pmt_dict_add(d_header, mp("version"), mp(METADATA_VERSION));
  d_header = pmt_dict_add(d_header, mp("rx_rate"), mp(samp_rate));
  d_header = pmt_dict_add(d_header, mp("rx_time"), timestamp);
  d_header = pmt_dict_add(d_header, mp("type"), pmt_from_long(type));
  d_header = pmt_dict_add(d_header, mp("cplx"), complex ? PMT_T : PMT_F);
  d_header = pmt_dict_add(d_header, mp("strt"), pmt_from_uint64(METADATA_HEADER_SIZE+d_extra_size));
  d_header = pmt_dict_add(d_header, mp("size"), pmt_from_uint64(0));

  write_header(d_header, d_extra);
}

void
gr_file_meta_sink::write_header(pmt_t header, pmt_t extra)
{
  do_update();

  std::string header_str = pmt_serialize_str(header);
  std::string extra_str = pmt_serialize_str(extra);

  if((header_str.size() != METADATA_HEADER_SIZE) && (extra_str.size() != d_extra_size))
      throw std::runtime_error("file_meta_sink: header or extras is wrong size.\n");

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

  nwritten = 0;
  while(nwritten < extra_str.size()) {
    std::string sub = extra_str.substr(nwritten);
    int count = fwrite(sub.c_str(), sizeof(char), sub.size(), d_fp);
    nwritten += count;
    if((count == 0) && (ferror(d_fp))) {
      fclose(d_fp);
      throw std::runtime_error("file_meta_sink: error writing extra to file.\n");
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
  else if(pmt_dict_has_key(d_extra, key)) {
    d_extra = pmt_dict_add(d_extra, key, value);
    return true;
  }
  else {
    return false;
  }
}

void
gr_file_meta_sink::update_last_header()
{
  // Update the last header info with the number of samples this
  // block represents.
  size_t hdrlen = METADATA_HEADER_SIZE+d_extra_size;
  size_t seg_size = d_itemsize*d_total_seg_size;
  pmt_t s = pmt_from_uint64(seg_size);
  if(update_header(mp("size"), s)) {
    fseek(d_fp, -seg_size-hdrlen, SEEK_CUR);
    write_header(d_header, d_extra);
    fseek(d_fp, seg_size, SEEK_CUR);
  }
}

void
gr_file_meta_sink::write_and_update()
{
  // New header, so set current size of chunk to 0 and start of chunk
  // based on current index + header size.
  //uint64_t loc = get_last_header_loc();
  pmt_t s = pmt_from_uint64(0);
  if(update_header(mp("size"), s)) {
    // If we have multiple tags on the same offset, this makes
    // sure we just overwrite the same header each time instead
    // of creating a new header per tag.
    s = pmt_from_uint64(METADATA_HEADER_SIZE + d_extra_size);
    if(update_header(mp("strt"), s)) {
      write_header(d_header, d_extra);
    }
  }
}

gr_file_meta_sink::~gr_file_meta_sink()
{
  update_last_header();
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
	write_header(d_header, d_extra);
      }
    }
    else {
      update_last_header();
      if(update_header(itr->key, itr->value)) {
	write_and_update();
      }
    }
  }

  while(nwritten < noutput_items) {
    size_t towrite = std::min(d_max_seg_size - d_total_seg_size,
			      (size_t)(noutput_items - nwritten));
    int count = fwrite(inbuf, d_itemsize, towrite, d_fp);
    if(count == 0)	// FIXME add error handling
      break;
    nwritten += count;
    inbuf += count * d_itemsize;

    d_total_seg_size += count;
    if(d_total_seg_size == d_max_seg_size) {
      update_last_header();
      write_and_update();
      d_total_seg_size = 0;
    }
  }

  if(d_unbuffered)
    fflush(d_fp);

  return nwritten;
}
