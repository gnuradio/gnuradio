/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
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

#include "tagged_file_sink_impl.h"
#include <gnuradio/io_signature.h>
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

namespace gr {
  namespace blocks {

    tagged_file_sink::sptr
    tagged_file_sink::make(size_t itemsize, double samp_rate)
    {
      return gnuradio::get_initial_sptr
	(new tagged_file_sink_impl(itemsize, samp_rate));
    }

    tagged_file_sink_impl::tagged_file_sink_impl(size_t itemsize, double samp_rate)
      : sync_block("tagged_file_sink",
		      io_signature::make(1, 1, itemsize),
		      io_signature::make(0, 0, 0)),
	d_itemsize (itemsize), d_n(0), d_sample_rate(samp_rate)
    {
      d_state = NOT_IN_BURST;
      d_last_N = 0;
      d_timeval = 0;
    }

    tagged_file_sink_impl::~tagged_file_sink_impl()
    {
    }

    int
    tagged_file_sink_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      char *inbuf = (char*)input_items[0];

      uint64_t start_N = nitems_read(0);
      uint64_t end_N = start_N + (uint64_t)(noutput_items);
      pmt::pmt_t bkey = pmt::string_to_symbol("burst");
      pmt::pmt_t tkey = pmt::string_to_symbol("rx_time"); // use gr_tags::key_time

      std::vector<tag_t> all_tags;
      get_tags_in_range(all_tags, 0, start_N, end_N);

      std::sort(all_tags.begin(), all_tags.end(), tag_t::offset_compare);

      std::vector<tag_t>::iterator vitr = all_tags.begin();

      // Look for a time tag and initialize d_timeval.
      std::vector<tag_t> time_tags_outer;
      get_tags_in_range(time_tags_outer, 0, start_N, end_N, tkey);
      if(time_tags_outer.size() > 0) {
	const tag_t tag = time_tags_outer[0];
	uint64_t offset = tag.offset;
	pmt::pmt_t time = tag.value;
	uint64_t tsecs = pmt::to_uint64(pmt::tuple_ref(time, 0));
	double tfrac = pmt::to_double(pmt::tuple_ref(time, 1));
	double delta = (double)offset / d_sample_rate;
	d_timeval = (double)tsecs + tfrac + delta;
	d_last_N = offset;
      }

      int idx = 0, idx_stop = 0;
      while(idx < noutput_items) {
	if(d_state == NOT_IN_BURST) {
	  while(vitr != all_tags.end()) {
	    if((pmt::eqv((*vitr).key, bkey)) &&
	       pmt::is_true((*vitr).value)) {

	      uint64_t N = (*vitr).offset;
	      idx = (int)(N - start_N);

	      //std::cout << std::endl << "Found start of burst: "
	      //	    << idx << ", " << N << std::endl;

	      // Find time burst occurred by getting latest time tag and extrapolating
	      // to new time based on sample rate of this block.
	      std::vector<tag_t> time_tags;
	      //get_tags_in_range(time_tags, 0, d_last_N, N, gr_tags::key_time);
	      get_tags_in_range(time_tags, 0, d_last_N, N, tkey);
	      if(time_tags.size() > 0) {
		const tag_t tag = time_tags[time_tags.size()-1];

		uint64_t time_nitems = tag.offset;

		// Get time based on last time tag from USRP
		pmt::pmt_t time = tag.value;
		uint64_t tsecs = pmt::to_uint64(pmt::tuple_ref(time, 0));
		double tfrac = pmt::to_double(pmt::tuple_ref(time, 1));

		// Get new time from last time tag + difference in time to when
		// burst tag occurred based on the sample rate
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
	      filename << "file" << unique_id() << "_" << d_n << "_" << d_timeval << ".dat";
	      d_n++;

	      int fd;
	      if((fd = ::open(filename.str().c_str(),
			      O_WRONLY|O_CREAT|O_TRUNC|OUR_O_LARGEFILE|OUR_O_BINARY,
			      0664)) < 0){
		perror(filename.str().c_str());
		return -1;
	      }

	      // FIXME:
	      //if((d_handle = fdopen (fd, d_is_binary ? "wb" : "w")) == NULL) {
	      if((d_handle = fdopen (fd, "wb")) == NULL) {
		perror(filename.str().c_str());
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
	    if((pmt::eqv((*vitr).key, bkey)) &&
	       pmt::is_false((*vitr).value)) {
	      uint64_t N = (*vitr).offset;
	      idx_stop = (int)N - start_N;

	      //std::cout << "Found end of burst: "
	      //	    << idx_stop << ", " << N << std::endl;

	      int count = fwrite (&inbuf[d_itemsize*idx], d_itemsize,
				  idx_stop-idx, d_handle);
	      if(count == 0) {
		if(ferror(d_handle)) {
		  perror("tagged_file_sink: error writing file");
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
		perror("tagged_file_sink: error writing file");
	      }
	    }
	    idx = noutput_items;
	  }
	}
      }

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
