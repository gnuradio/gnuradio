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

#include "file_meta_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>

// win32 (mingw/msvc) specific
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


    file_meta_sink::sptr
    file_meta_sink::make(size_t itemsize, const std::string &filename,
			 double samp_rate, double relative_rate,
			 gr_file_types type, bool complex,
			 size_t max_segment_size,
			 const std::string &extra_dict,
			 bool detached_header)
    {
      return gnuradio::get_initial_sptr
	(new file_meta_sink_impl(itemsize, filename,
				 samp_rate, relative_rate,
				 type, complex,
				 max_segment_size,
				 extra_dict,
				 detached_header));
    }

    file_meta_sink_impl::file_meta_sink_impl(size_t itemsize,
					     const std::string &filename,
					     double samp_rate, double relative_rate,
					     gr_file_types type, bool complex,
					     size_t max_segment_size,
					     const std::string &extra_dict,
					     bool detached_header)
      : sync_block("file_meta_sink",
		      io_signature::make(1, 1, itemsize),
		      io_signature::make(0, 0, 0)),
	d_itemsize(itemsize),
	d_samp_rate(samp_rate), d_relative_rate(relative_rate),
	d_max_seg_size(max_segment_size), d_total_seg_size(0),
	d_updated(false), d_unbuffered(false)
    {
      d_fp = 0;
      d_new_fp = 0;
      d_hdr_fp = 0;
      d_new_hdr_fp = 0;

      if(detached_header == true)
	d_state = STATE_DETACHED;
      else
	d_state = STATE_INLINE;

      if(!open(filename))
	throw std::runtime_error("file_meta_sink: can't open file\n");

      pmt::pmt_t timestamp = pmt::make_tuple(pmt::from_uint64(0),
					     pmt::from_double(0));

      // handle extra dictionary
      d_extra = pmt::make_dict();
      if(extra_dict.size() > 0) {
	pmt::pmt_t extras = pmt::deserialize_str(extra_dict);
	pmt::pmt_t keys = pmt::dict_keys(extras);
	pmt::pmt_t vals = pmt::dict_values(extras);
	size_t nitems = pmt::length(keys);
	for(size_t i = 0; i < nitems; i++) {
	  d_extra = pmt::dict_add(d_extra,
				  pmt::nth(i, keys),
				  pmt::nth(i, vals));
	}
      }

      d_extra_size = pmt::serialize_str(d_extra).size();

      d_header = pmt::make_dict();
      d_header = pmt::dict_add(d_header, pmt::mp("version"), pmt::mp(METADATA_VERSION));
      d_header = pmt::dict_add(d_header, pmt::mp("rx_rate"), pmt::mp(samp_rate));
      d_header = pmt::dict_add(d_header, pmt::mp("rx_time"), timestamp);
      d_header = pmt::dict_add(d_header, pmt::mp("size"), pmt::from_long(d_itemsize));
      d_header = pmt::dict_add(d_header, pmt::mp("type"), pmt::from_long(type));
      d_header = pmt::dict_add(d_header, pmt::mp("cplx"), complex ? pmt::PMT_T : pmt::PMT_F);
      d_header = pmt::dict_add(d_header, pmt::mp("strt"), pmt::from_uint64(METADATA_HEADER_SIZE+d_extra_size));
      d_header = pmt::dict_add(d_header, mp("bytes"), pmt::from_uint64(0));

      do_update();

      if(d_state == STATE_DETACHED)
	write_header(d_hdr_fp, d_header, d_extra);
      else
	write_header(d_fp, d_header, d_extra);
    }

    file_meta_sink_impl::~file_meta_sink_impl()
    {
      close();


    }

    bool
    file_meta_sink_impl::open(const std::string &filename)
    {
      bool ret = true;
      if(d_state == STATE_DETACHED) {
	std::string s = filename + ".hdr";
	ret = _open(&d_new_hdr_fp, s.c_str());
      }

      ret = ret && _open(&d_new_fp, filename.c_str());
      d_updated = true;
      return ret;
    }

    bool
    file_meta_sink_impl::_open(FILE **fp, const char *filename)
    {
      gr::thread::scoped_lock guard(d_setlock); // hold mutex for duration of this function

      bool ret = true;
      int fd;

      if((fd = ::open(filename,
		      O_WRONLY|O_CREAT|O_TRUNC|OUR_O_LARGEFILE|OUR_O_BINARY,
		      0664)) < 0){
	perror(filename);
	return false;
      }

      if(*fp) {		// if we've already got a new one open, close it
	fclose(*fp);
	fp = 0;
      }

      if((*fp = fdopen(fd, "wb")) == NULL) {
	perror(filename);
	::close(fd);		// don't leak file descriptor if fdopen fails.
      }

      ret = fp != 0;

      return ret;
    }

    void
    file_meta_sink_impl::close()
    {
      gr::thread::scoped_lock guard(d_setlock); // hold mutex for duration of this function
      update_last_header();

      if(d_state == STATE_DETACHED) {
	if(d_new_hdr_fp) {
	  fclose(d_new_hdr_fp);
	  d_new_hdr_fp = 0;
	}
      }

      if(d_new_fp) {
	fclose(d_new_fp);
	d_new_fp = 0;
      }
      d_updated = true;

      if (d_fp) {
        fclose(d_fp);
        d_fp = 0;
      }

      if (d_state == STATE_DETACHED) {
        if (d_hdr_fp) {
          fclose(d_hdr_fp);
          d_hdr_fp = 0;
        }
      }
    }

    void
    file_meta_sink_impl::do_update()
    {
      if(d_updated) {
	gr::thread::scoped_lock guard(d_setlock); // hold mutex for duration of this block
	if(d_state == STATE_DETACHED) {
	  if(d_hdr_fp)
	    fclose(d_hdr_fp);
	  d_hdr_fp = d_new_hdr_fp;		// install new file pointer
	  d_new_hdr_fp = 0;
	}

	if(d_fp)
	  fclose(d_fp);
	d_fp = d_new_fp;		// install new file pointer
	d_new_fp = 0;

	d_updated = false;
      }
    }

    void
    file_meta_sink_impl::write_header(FILE *fp, pmt::pmt_t header, pmt::pmt_t extra)
    {
      std::string header_str = pmt::serialize_str(header);
      std::string extra_str = pmt::serialize_str(extra);

      if((header_str.size() != METADATA_HEADER_SIZE) && (extra_str.size() != d_extra_size))
	throw std::runtime_error("file_meta_sink: header or extras is wrong size.\n");

      size_t nwritten = 0;
      while(nwritten < header_str.size()) {
	std::string sub = header_str.substr(nwritten);
	int count = fwrite(sub.c_str(), sizeof(char), sub.size(), fp);
	nwritten += count;
	if((count == 0) && (ferror(fp))) {
	  fclose(fp);
	  throw std::runtime_error("file_meta_sink: error writing header to file.\n");
	}
      }

      nwritten = 0;
      while(nwritten < extra_str.size()) {
	std::string sub = extra_str.substr(nwritten);
	int count = fwrite(sub.c_str(), sizeof(char), sub.size(), fp);
	nwritten += count;
	if((count == 0) && (ferror(fp))) {
	  fclose(fp);
	  throw std::runtime_error("file_meta_sink: error writing extra to file.\n");
	}
      }

      fflush(fp);
    }

    void
    file_meta_sink_impl::update_header(pmt::pmt_t key, pmt::pmt_t value)
    {
      // Special handling caveat to transform rate from radio source into
      // the rate at this sink.
      if(pmt::eq(key, mp("rx_rate"))) {
	d_samp_rate = pmt::to_double(value);
	value = pmt::from_double(d_samp_rate*d_relative_rate);
      }

      // If the tag is not part of the standard header, we put it into the
      // extra data, which either updates the current dictionary or adds a
      // new item.
      if(pmt::dict_has_key(d_header, key)) {
	d_header = pmt::dict_add(d_header, key, value);
      }
      else {
	d_extra = pmt::dict_add(d_extra, key, value);
	d_extra_size = pmt::serialize_str(d_extra).size();
      }
    }

    void
    file_meta_sink_impl::update_last_header()
    {
      if(d_state == STATE_DETACHED) {
        if (d_hdr_fp) update_last_header_detached();
      } else {
        if(d_fp) update_last_header_inline();
      }
    }

    void
    file_meta_sink_impl::update_last_header_inline()
    {
      // Update the last header info with the number of samples this
      // block represents.

      size_t hdrlen = pmt::to_uint64(pmt::dict_ref(d_header, mp("strt"), pmt::PMT_NIL));
      size_t seg_size = d_itemsize*d_total_seg_size;
      pmt::pmt_t s = pmt::from_uint64(seg_size);
      update_header(mp("bytes"), s);
      update_header(mp("strt"), pmt::from_uint64(METADATA_HEADER_SIZE+d_extra_size));
      fseek(d_fp, -seg_size-hdrlen, SEEK_CUR);
      write_header(d_fp, d_header, d_extra);
      fseek(d_fp, seg_size, SEEK_CUR);
    }

    void
    file_meta_sink_impl::update_last_header_detached()
    {
      // Update the last header info with the number of samples this
      // block represents.
      size_t hdrlen = pmt::to_uint64(pmt::dict_ref(d_header, mp("strt"), pmt::PMT_NIL));
      size_t seg_size = d_itemsize*d_total_seg_size;
      pmt::pmt_t s = pmt::from_uint64(seg_size);
      update_header(mp("bytes"), s);
      update_header(mp("strt"), pmt::from_uint64(METADATA_HEADER_SIZE+d_extra_size));
      fseek(d_hdr_fp, -hdrlen, SEEK_CUR);
      write_header(d_hdr_fp, d_header, d_extra);
    }

    void
    file_meta_sink_impl::write_and_update()
    {
      // New header, so set current size of chunk to 0 and start of chunk
      // based on current index + header size.
      //uint64_t loc = get_last_header_loc();
      pmt::pmt_t s = pmt::from_uint64(0);
      update_header(mp("bytes"), s);

      // If we have multiple tags on the same offset, this makes
      // sure we just overwrite the same header each time instead
      // of creating a new header per tag.
      s = pmt::from_uint64(METADATA_HEADER_SIZE + d_extra_size);
      update_header(mp("strt"), s);

      if(d_state == STATE_DETACHED)
	write_header(d_hdr_fp, d_header, d_extra);
      else
	write_header(d_fp, d_header, d_extra);
    }

    void
    file_meta_sink_impl::update_rx_time()
    {
      pmt::pmt_t rx_time = pmt::string_to_symbol("rx_time");
      pmt::pmt_t r = pmt::dict_ref(d_header, rx_time, pmt::PMT_NIL);
      uint64_t secs = pmt::to_uint64(pmt::tuple_ref(r, 0));
      double fracs = pmt::to_double(pmt::tuple_ref(r, 1));
      double diff = d_total_seg_size / (d_samp_rate*d_relative_rate);

      //std::cerr << "old secs:  " << secs << std::endl;
      //std::cerr << "old fracs: " << fracs << std::endl;
      //std::cerr << "seg size:  " << d_total_seg_size << std::endl;
      //std::cerr << "diff:      " << diff << std::endl;

      fracs += diff;
      uint64_t new_secs = static_cast<uint64_t>(fracs);
      secs += new_secs;
      fracs -= new_secs;

      //std::cerr << "new secs:  " << secs << std::endl;
      //std::cerr << "new fracs: " << fracs << std::endl << std::endl;

      r = pmt::make_tuple(pmt::from_uint64(secs), pmt::from_double(fracs));
      d_header = pmt::dict_add(d_header, rx_time, r);
    }

    int
    file_meta_sink_impl::work(int noutput_items,
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
      std::vector<tag_t> all_tags;
      get_tags_in_range(all_tags, 0, abs_N, end_N);

      std::vector<tag_t>::iterator itr;
      for(itr = all_tags.begin(); itr != all_tags.end(); itr++) {
	int item_offset = (int)(itr->offset - abs_N);

	// Write date to file up to the next tag location
	while(nwritten < item_offset) {
	  size_t towrite = std::min(d_max_seg_size - d_total_seg_size,
				    (size_t)(item_offset - nwritten));
	  int count = fwrite(inbuf, d_itemsize, towrite, d_fp);
	  if(count == 0)	// FIXME add error handling
	    break;
	  nwritten += count;
	  inbuf += count * d_itemsize;

	  d_total_seg_size += count;

	  // Only add a new header if we are not at the position of the
	  // next tag
	  if((d_total_seg_size == d_max_seg_size) &&
	     (nwritten < item_offset)) {
	    update_last_header();
	    update_rx_time();
	    write_and_update();
	    d_total_seg_size = 0;
	  }
	}

	if(d_total_seg_size > 0) {
	  update_last_header();
	  update_header(itr->key, itr->value);
	  write_and_update();
	  d_total_seg_size = 0;
	}
	else {
	  update_header(itr->key, itr->value);
	  update_last_header();
	}
      }

      // Finish up the rest of the data after tags
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
	  update_rx_time();
	  write_and_update();
	  d_total_seg_size = 0;
	}
      }

      if(d_unbuffered)
	fflush(d_fp);

      return nwritten;
    }

  } /* namespace blocks */
} /* namespace gr */
