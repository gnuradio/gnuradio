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

#include "file_meta_source_impl.h"
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


    file_meta_source::sptr
    file_meta_source::make(const std::string &filename,
			   bool repeat,
			   bool detached_header,
			   const std::string &hdr_filename)
    {
      return gnuradio::get_initial_sptr
	(new file_meta_source_impl(filename,
				   repeat,
				   detached_header,
				   hdr_filename));
    }

    file_meta_source_impl::file_meta_source_impl(const std::string &filename,
						 bool repeat,
						 bool detached_header,
						 const std::string &hdr_filename)
      : sync_block("file_meta_source",
		      io_signature::make(0, 0, 0),
		      io_signature::make(1, 1, 1)),
	d_itemsize(0), d_samp_rate(0),
	d_seg_size(0),
	d_updated(false), d_repeat(repeat)
    {
      d_fp = 0;
      d_new_fp = 0;
      d_hdr_fp = 0;
      d_new_hdr_fp = 0;

      if(detached_header == true) {
	d_state = STATE_DETACHED;
      }
      else
	d_state = STATE_INLINE;

      if(!open(filename, hdr_filename))
	throw std::runtime_error("file_meta_source: can't open file\n");

      do_update();

      pmt::pmt_t hdr = pmt::PMT_NIL, extras = pmt::PMT_NIL;
      if(read_header(hdr, extras)) {
	parse_header(hdr, 0, d_tags);
	parse_extras(extras, 0, d_tags);
      }
      else
	throw std::runtime_error("file_meta_source: could not read header.\n");

      // Set output signature based on itemsize info in header
      set_output_signature(io_signature::make(1, 1, d_itemsize));
    }

    file_meta_source_impl::~file_meta_source_impl()
    {
      close();


    }

    bool
    file_meta_source_impl::read_header(pmt::pmt_t &hdr, pmt::pmt_t &extras)
    {
      // Select which file handle to read from.
      FILE *fp;
      if(d_state == STATE_DETACHED)
	fp = d_hdr_fp;
      else
	fp = d_fp;

      size_t ret;
      size_t size = 0;
      std::string str;
      char *hdr_buffer = new char[METADATA_HEADER_SIZE];
      while(size < METADATA_HEADER_SIZE) {
	ret = fread(&hdr_buffer[size], sizeof(char), METADATA_HEADER_SIZE-size, fp);
	if(ret == 0) {
	  delete [] hdr_buffer;
	  if(feof(fp))
	    return false;
	  else {
	    std::stringstream s;
	    s << "file_meta_source: error occurred extracting header: "
	      << strerror(errno) << std::endl;
	    throw std::runtime_error(s.str());
	  }
	}
	size += ret;
      }

      // Convert to string or the char array gets confused by the \0
      str.insert(0, hdr_buffer, METADATA_HEADER_SIZE);
      hdr = pmt::deserialize_str(str);
      delete [] hdr_buffer;

      uint64_t seg_start, extra_len = 0;
      pmt::pmt_t r, dump;
      if(pmt::dict_has_key(hdr, pmt::string_to_symbol("strt"))) {
	r = pmt::dict_ref(hdr, pmt::string_to_symbol("strt"), dump);
	seg_start = pmt::to_uint64(r);
	extra_len = seg_start - METADATA_HEADER_SIZE;
      }

      if(extra_len > 0) {
	size = 0;
	hdr_buffer = new char[extra_len];
	while(size < extra_len) {
	  ret = fread(&hdr_buffer[size], sizeof(char), extra_len-size, fp);
	  if(ret == 0) {
	    delete [] hdr_buffer;
	    if(feof(fp))
	      return false;
	    else {
	      std::stringstream s;
	      s << "file_meta_source: error occurred extracting extras: "
		<< strerror(errno) << std::endl;
	      throw std::runtime_error(s.str());
	    }
	  }
	  size += ret;
	}

	str.clear();
	str.insert(0, hdr_buffer, extra_len);
	extras = pmt::deserialize_str(str);
	delete [] hdr_buffer;
      }

      return true;
    }

    void
    file_meta_source_impl::parse_header(pmt::pmt_t hdr, uint64_t offset,
					std::vector<tag_t> &tags)
    {
      pmt::pmt_t r, key;

      // GET SAMPLE RATE
      key = pmt::string_to_symbol("rx_rate");
      if(pmt::dict_has_key(hdr, key)) {
	r = pmt::dict_ref(hdr, key, pmt::PMT_NIL);
	d_samp_rate = pmt::to_double(r);

	tag_t t;
	t.offset = offset;
	t.key = key;
	t.value = r;
	t.srcid = alias_pmt();
	tags.push_back(t);
      }
      else {
	throw std::runtime_error("file_meta_source: Could not extract sample rate.\n");
      }

      // GET TIME STAMP
      key = pmt::string_to_symbol("rx_time");
      if(pmt::dict_has_key(hdr, key)) {
	d_time_stamp = pmt::dict_ref(hdr, key, pmt::PMT_NIL);

	tag_t t;
	t.offset = offset;
	t.key = key;
	t.value = d_time_stamp;
	t.srcid = alias_pmt();
	tags.push_back(t);
      }
      else {
	throw std::runtime_error("file_meta_source: Could not extract time stamp.\n");
      }

      // GET ITEM SIZE OF DATA
      if(pmt::dict_has_key(hdr, pmt::string_to_symbol("size"))) {
	d_itemsize = pmt::to_long(pmt::dict_ref(hdr, pmt::string_to_symbol("size"), pmt::PMT_NIL));
      }
      else {
	throw std::runtime_error("file_meta_source: Could not extract item size.\n");
      }

      // GET SEGMENT SIZE
      if(pmt::dict_has_key(hdr, pmt::string_to_symbol("bytes"))) {
	d_seg_size = pmt::to_uint64(pmt::dict_ref(hdr, pmt::string_to_symbol("bytes"), pmt::PMT_NIL));

	// Convert from bytes to items
	d_seg_size /= d_itemsize;
      }
      else {
	throw std::runtime_error("file_meta_source: Could not extract segment size.\n");
      }
    }

    void
    file_meta_source_impl::parse_extras(pmt::pmt_t extras, uint64_t offset,
					std::vector<tag_t> &tags)
    {
      pmt::pmt_t item, key, val;

      size_t nitems = pmt::length(extras);
      for(size_t i = 0; i < nitems; i++) {
	item = pmt::nth(i, extras);
	key = pmt::car(item);
	val = pmt::cdr(item);

	tag_t t;
	t.offset = offset;
	t.key = key;
	t.value = val;
	t.srcid = alias_pmt();
	tags.push_back(t);
      }
    }

    bool
    file_meta_source_impl::open(const std::string &filename,
				const std::string &hdr_filename)
    {
      bool ret = true;
      if(d_state == STATE_DETACHED) {
	std::string s;
	if(hdr_filename == "")
	  s = filename + ".hdr";
	else
	  s = hdr_filename;
	ret = _open(&d_new_hdr_fp, s.c_str());
      }

      ret = ret && _open(&d_new_fp, filename.c_str());
      d_updated = true;
      return ret;
    }

    bool
    file_meta_source_impl::_open(FILE **fp, const char *filename)
    {
      gr::thread::scoped_lock guard(d_setlock); // hold mutex for duration of this function

      bool ret = true;
      int fd;

      if((fd = ::open(filename,
		      O_RDONLY|OUR_O_LARGEFILE|OUR_O_BINARY)) < 0) {
	perror(filename);
	return false;
      }

      if(*fp) {		// if we've already got a new one open, close it
	fclose(*fp);
	fp = 0;
      }

      if((*fp = fdopen(fd, "rb")) == NULL) {
	perror(filename);
	::close(fd);		// don't leak file descriptor if fdopen fails.
      }

      ret = fp != 0;

      return ret;
    }

    void
    file_meta_source_impl::close()
    {
      gr::thread::scoped_lock guard(d_setlock); // hold mutex for duration of this function
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
    file_meta_source_impl::do_update()
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

    int
    file_meta_source_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      // We've reached the end of a segment; parse the next header and get
      // the new tags to send and set the next segment size.
      if(d_seg_size == 0) {
	pmt::pmt_t hdr=pmt::PMT_NIL, extras=pmt::PMT_NIL;
	if(read_header(hdr, extras)) {
	  parse_header(hdr, nitems_written(0), d_tags);
	  parse_extras(extras, nitems_written(0), d_tags);
	}
	else {
          if(!d_repeat)
            return -1;
          else {
            if(fseek(d_fp, 0, SEEK_SET) == -1) {
              std::stringstream s;
              s << "[" << __FILE__ << "]" << " fseek failed" << std::endl;
              throw std::runtime_error(s.str());
            }
          }
	}
      }

      char *out = (char*)output_items[0];
      int i;
      int seg_size = std::min(noutput_items, (int)d_seg_size);
      int size = seg_size;

      do_update();       // update d_fp is reqd
      if(d_fp == NULL)
	throw std::runtime_error("work with file not open");

      // Push all tags onto the stream and remove them from the vector
      while(!d_tags.empty()) {
	add_item_tag(0, d_tags.back());
	d_tags.pop_back();
      }

      gr::thread::scoped_lock lock(d_setlock); // hold for the rest of this function
      while(size) {
	i = fread(out, d_itemsize, size, d_fp);

	size -= i;
	d_seg_size -= i;
	out += i * d_itemsize;

	if(size == 0)		// done
          break;

	if(i > 0)			// short read, try again
	  continue;

	// We got a zero from fread.  This is either EOF or error.  In
	// any event, if we're in repeat mode, seek back to the beginning
	// of the file and try again, else break

	if(!d_repeat)
	  break;

	if(fseek(d_fp, 0, SEEK_SET) == -1) {
	  std::stringstream s;
	  s << "[" << __FILE__ << "]" << " fseek failed" << std::endl;
	  throw std::runtime_error(s.str());
	}
      }

      if(size > 0) {			// EOF or error
	if(size == seg_size)    	// we didn't read anything; say we're done
	  return -1;
	return seg_size - size;	// else return partial result
      }

      return seg_size;
    }

  } /* namespace blocks */
} /* namespace gr */
