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
		       double samp_rate, gr_file_types type, bool complex,
		       const std::string &extra_dict)
{
  return gnuradio::get_initial_sptr
    (new gr_file_meta_sink(itemsize, filename,
			   samp_rate, type, complex,
			   extra_dict));
}

gr_file_meta_sink::gr_file_meta_sink(size_t itemsize, const char *filename,
				     double samp_rate, gr_file_types type, bool complex,
				     const std::string &extra_dict)
  : gr_sync_block("file_meta_sink",
		  gr_make_io_signature(1, 1, itemsize),
		  gr_make_io_signature(0, 0, 0)),
    gr_file_sink_base(filename, true),
    d_itemsize(itemsize)
{
  if(!open(filename))
    throw std::runtime_error("file_meta_sink: can't open file\n");

  pmt_t timestamp = pmt_make_tuple(pmt_from_uint64(0),
				   pmt_from_double(0));

  d_header = pmt_make_dict();
  d_header = pmt_dict_add(d_header, mp("sr"), mp(samp_rate));
  d_header = pmt_dict_add(d_header, mp("time"), timestamp);
  d_header = pmt_dict_add(d_header, mp("type"), pmt_from_long(type));
  d_header = pmt_dict_add(d_header, mp("cplx"), complex ? PMT_T : PMT_F);
  d_header = pmt_dict_add(d_header, mp("strt"), pmt_from_uint64(109));
  d_header = pmt_dict_add(d_header, mp("size"), pmt_from_uint64(0));
  // handle extra dictionary

  write_header(d_header);
}

void
gr_file_meta_sink::write_header(pmt_t header)
{
  do_update();

  std::string header_str = pmt_serialize_str(header);

  size_t nwritten = 0;
  while(nwritten < header_str.size()) {
    std::string sub = header_str.substr(nwritten);
    int count = fwrite(sub.c_str(), sizeof(char), sub.size(), d_fp);
    nwritten += count;
    if((count == 0) && (ferror(d_fp))) {
      fclose(d_fp);
      throw std::runtime_error("file_meta_sink: error writing header to file\n");
    }
  }
}

gr_file_meta_sink::~gr_file_meta_sink()
{
  fseek(d_fp, 0, SEEK_SET);

  // Replace the dictionary item with the data size now that we're
  // done.
  uint64_t s = nitems_read(0) * d_itemsize;
  d_header = pmt_dict_delete(d_header, mp("size"));
  d_header = pmt_dict_add(d_header, mp("size"), pmt_from_uint64(s));
  write_header(d_header);
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
