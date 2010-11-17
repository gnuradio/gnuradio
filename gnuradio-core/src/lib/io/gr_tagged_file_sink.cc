/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <gr_tagged_file_sink.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <iostream>

#ifdef O_BINARY
#define	OUR_O_BINARY O_BINARY
#else
#define	OUR_O_BINARY 0
#endif

// should be handled via configure
#ifdef O_LARGEFILE
#define	OUR_O_LARGEFILE	O_LARGEFILE
#else
#define	OUR_O_LARGEFILE 0
#endif


gr_tagged_file_sink::gr_tagged_file_sink (size_t itemsize)
  : gr_sync_block ("tagged_file_sink",
		   gr_make_io_signature (1, 1, itemsize),
		   gr_make_io_signature (0, 0, 0)),
    d_itemsize (itemsize),d_n(0)
{
  d_state = NOT_IN_BURST;
}

gr_tagged_file_sink_sptr
gr_make_tagged_file_sink (size_t itemsize)
{
  return gnuradio::get_initial_sptr(new gr_tagged_file_sink (itemsize));
}

gr_tagged_file_sink::~gr_tagged_file_sink ()
{
}

bool pmtcompare(pmt::pmt_t x, pmt::pmt_t y)
{
  uint64_t t_x, t_y;
  t_x = pmt::pmt_to_uint64(pmt::pmt_tuple_ref(x, 0));
  t_y = pmt::pmt_to_uint64(pmt::pmt_tuple_ref(y, 0));
  return t_x < t_y;
}

int 
gr_tagged_file_sink::work (int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
{
  char *inbuf = (char *) input_items[0];

  uint64_t start_N = nitems_read(0);
  uint64_t end_N = start_N + (uint64_t)(noutput_items);
  pmt::pmt_t bkey = pmt::pmt_string_to_symbol("burst");
  pmt::pmt_t tkey = pmt::pmt_string_to_symbol("packet_time_stamp");
  std::vector<pmt::pmt_t> all_tags = get_tags_in_range(0, start_N, end_N);
  std::sort(all_tags.begin(), all_tags.end(), pmtcompare);
  std::cout << "Number of tags: " << all_tags.size() << std::endl;

  std::vector<pmt::pmt_t>::iterator vitr = all_tags.begin();

  int idx = 0, idx_stop = 0;
  while(idx < noutput_items) {
    if(d_state == NOT_IN_BURST) {
      while(vitr != all_tags.end()) {
	//std::cout << "\tNot in burst: " << *vitr << std::endl;

	if((pmt::pmt_eqv(pmt::pmt_tuple_ref(*vitr, 2), bkey)) &&
	   pmt::pmt_is_true(pmt::pmt_tuple_ref(*vitr,3))) {

	  uint64_t N = pmt::pmt_to_uint64(pmt::pmt_tuple_ref(*vitr, 0)) - start_N;
	  idx = (int)N;
	  
	  std::cout << std::endl << "Found start of burst: " << N << ", " << N+start_N << std::endl;
	  
	  std::stringstream filename;
	  filename << "file" << d_n << ".dat";
	  d_n++;
	  
	  int fd;
	  if ((fd = ::open (filename.str().c_str(),
			    O_WRONLY|O_CREAT|O_TRUNC|OUR_O_LARGEFILE|OUR_O_BINARY,
			    0664)) < 0){
	    perror (filename.str().c_str());
	    return -1;
	  }
	  
	  // FIXME:
	  //if ((d_handle = fdopen (fd, d_is_binary ? "wb" : "w")) == NULL){
	  if ((d_handle = fdopen (fd, "wb")) == NULL){
	    perror (filename.str().c_str());
	    ::close(fd);		// don't leak file descriptor if fdopen fails.
	  }

	  std::cout << "Created new file: " << filename.str() << std::endl;

	  d_state = IN_BURST;
	  break;
	}
	
	vitr++;
      }
      if(d_state == NOT_IN_BURST)
	return noutput_items;
    }
    else {  // In burst
      while(vitr != all_tags.end()) {
	//std::cout << "\tin burst: " << *vitr << std::endl;

	if((pmt::pmt_eqv(pmt::pmt_tuple_ref(*vitr, 2), bkey)) &&
	   pmt::pmt_is_false(pmt::pmt_tuple_ref(*vitr,3))) {
	  uint64_t N = pmt::pmt_to_uint64(pmt::pmt_tuple_ref(*vitr, 0)) - start_N;
	  idx_stop = (int)N;

	  std::cout << "Found end of burst: " << N << ", " << N+start_N << std::endl;

	  int count = fwrite (&inbuf[d_itemsize*idx], d_itemsize, idx_stop-idx, d_handle);
	  if (count == 0)
	    break;
	  idx = idx_stop;
	  d_state = NOT_IN_BURST;
	  vitr++;
	  fclose(d_handle);
	  break;
	} else {
	  vitr++;
	}
      }
      if(d_state == IN_BURST) {
	std::cout << "writing part of burst: " << noutput_items-idx << std::endl;
	int count = fwrite (&inbuf[idx], d_itemsize, noutput_items-idx, d_handle);
	if (count == 0)
	  break;
	idx = noutput_items;
      }
      
    }
  }

  return noutput_items;
}
