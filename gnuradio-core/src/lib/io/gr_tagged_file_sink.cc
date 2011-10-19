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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <iostream>

#ifdef HAVE_IO_H
#include <io.h>
#endif

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


gr_tagged_file_sink::gr_tagged_file_sink (size_t itemsize, double samp_rate)
  : gr_sync_block ("tagged_file_sink",
		   gr_make_io_signature (1, 1, itemsize),
		   gr_make_io_signature (0, 0, 0)),
    d_itemsize (itemsize), d_n(0), d_sample_rate(samp_rate)
{
  d_state = NOT_IN_BURST;
  d_last_N = 0;
  d_timeval = 0;
}

gr_tagged_file_sink_sptr
gr_make_tagged_file_sink (size_t itemsize, double samp_rate)
{
  return gnuradio::get_initial_sptr(new gr_tagged_file_sink (itemsize, samp_rate));
}

gr_tagged_file_sink::~gr_tagged_file_sink ()
{
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
  //pmt::pmt_t tkey = pmt::pmt_string_to_symbol("time"); // use gr_tags::key_time

  std::vector<gr_tag_t> all_tags;
  get_tags_in_range(all_tags, 0, start_N, end_N);

  std::sort(all_tags.begin(), all_tags.end(), gr_tag_t::offset_compare);

  std::vector<gr_tag_t>::iterator vitr = all_tags.begin();

  int idx = 0, idx_stop = 0;
  while(idx < noutput_items) {
    if(d_state == NOT_IN_BURST) {
      while(vitr != all_tags.end()) {
	if((pmt::pmt_eqv((*vitr).key, bkey)) &&
	   pmt::pmt_is_true((*vitr).value)) {

	  uint64_t N = (*vitr).offset;
	  idx = (int)(N - start_N);

	  //std::cout << std::endl << "Found start of burst: "
	  //	    << idx << ", " << N << std::endl;

	  // Find time burst occurred by getting latest time tag and extrapolating
	  // to new time based on sample rate of this block.
	  std::vector<gr_tag_t> time_tags;
	  //get_tags_in_range(time_tags, 0, d_last_N, N, gr_tags::key_time);
	  get_tags_in_range(time_tags, 0, d_last_N, N,
			    pmt::pmt_string_to_symbol("time"));
	  if(time_tags.size() > 0) {
	    const gr_tag_t tag = time_tags[time_tags.size()-1];
	    
	    uint64_t time_nitems = tag.offset;

	    // Get time based on last time tag from USRP
	    pmt::pmt_t time = tag.value;
	    int tsecs = pmt::pmt_to_long(pmt::pmt_tuple_ref(time, 0));
	    double tfrac = pmt::pmt_to_double(pmt::pmt_tuple_ref(time, 1));

	    // Get new time from last time tag + difference in time to when
	    // burst tag occured based on the sample rate
	    double delta = (double)(N - time_nitems) / d_sample_rate;
	    d_timeval = (double)tsecs + tfrac + delta;

	    //std::cout.setf(std::ios::fixed, std::ios::floatfield);
	    //std::cout.precision(8);
	    //std::cout << "Time found: " << (double)tsecs + tfrac << std::endl;
	    //std::cout << "   time: " << d_timeval << std::endl;
	    //std::cout << "   time at N = " << time_nitems << " burst N = " << N << std::endl;
	  }
	  else {
	    // if no time tag, use last seen tag and update time based on
	    // sample rate of the block
	    d_timeval += (double)(N - d_last_N) / d_sample_rate;
	    //std::cout << "Time not found" << std::endl;
	    //std::cout << "   time: " << d_timeval << std::endl;
	  }
	  d_last_N = N;
	  
	  std::stringstream filename;
	  filename.setf(std::ios::fixed, std::ios::floatfield);
	  filename.precision(8);
	  filename << "file" << d_n << "_" << d_timeval << ".dat";
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

	  //std::cout << "Created new file: " << filename.str() << std::endl;

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
	if((pmt::pmt_eqv((*vitr).key, bkey)) &&
	   pmt::pmt_is_false((*vitr).value)) {
	  uint64_t N = (*vitr).offset;
	  idx_stop = (int)N - start_N;

	  //std::cout << "Found end of burst: "
	  //	    << idx_stop << ", " << N << std::endl;

	  int count = fwrite (&inbuf[d_itemsize*idx], d_itemsize,
			      idx_stop-idx, d_handle);
	  if (count == 0) {
	    if(ferror(d_handle)) {
	      perror("gr_tagged_file_sink: error writing file");
	    }
	  }
	  idx = idx_stop;
	  d_state = NOT_IN_BURST;
	  vitr++;
	  fclose(d_handle);
	  break;
	}
	else {
	  vitr++;
	}
      }
      if(d_state == IN_BURST) {
	int count = fwrite (&inbuf[d_itemsize*idx], d_itemsize,
			    noutput_items-idx, d_handle);
	if (count == 0) {
	  if(ferror(d_handle)) {
	    perror("gr_tagged_file_sink: error writing file");
	  }
	}
	idx = noutput_items;
      }
    }
  }

  return noutput_items;
}
