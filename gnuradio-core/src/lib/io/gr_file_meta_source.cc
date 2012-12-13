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

#include <gr_file_meta_source.h>
#include <gr_io_signature.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gruel/thread.h>
#include <stdexcept>

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

gr_file_meta_source_sptr
gr_make_file_meta_source(const char *filename,
			 bool repeat,
			 bool detached_header,
			 const char *hdr_filename)
{
  return gnuradio::get_initial_sptr
    (new gr_file_meta_source(filename,
			     repeat,
			     detached_header,
			     hdr_filename));
}

gr_file_meta_source::gr_file_meta_source(const char *filename,
					 bool repeat,
					 bool detached_header,
					 const char *hdr_filename)
  : gr_sync_block("file_meta_source",
		  gr_make_io_signature(0, 0, 0),
		  gr_make_io_signature(1, 1, 1)),
    d_itemsize(0), d_samp_rate(0),
    d_seg_size(0),
    d_updated(false), d_repeat(repeat)
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
    throw std::runtime_error("file_meta_source: can't open file\n");

  do_update();

  pmt_t hdr = PMT_NIL, extras = PMT_NIL;
  if(read_header(hdr, extras))
    parse_header(hdr);
  else
    throw std::runtime_error("file_meta_source: could not read header.\n");

  // Set output signature based on itemsize info in header
  set_output_signature(gr_make_io_signature(1, 1, d_itemsize));

  // Convert from bytes to items
  d_seg_size /= d_itemsize;
}

gr_file_meta_source::~gr_file_meta_source()
{
  close();

  if(d_fp) {
    fclose(d_fp);
    d_fp = 0;
  }
  
  if(d_state == STATE_DETACHED) {
    if(d_hdr_fp) {
      fclose(d_hdr_fp);
      d_hdr_fp = 0;
    }
  }
}

bool
gr_file_meta_source::read_header(pmt_t &hdr, pmt_t &extras)
{
  size_t ret;
  size_t size = 0;
  std::string str;
  char *hdr_buffer = new char[METADATA_HEADER_SIZE];
  while(size < METADATA_HEADER_SIZE) {
    ret = fread(&hdr_buffer[size], sizeof(char), METADATA_HEADER_SIZE-size, d_fp);
    if(ret == 0) {
      delete [] hdr_buffer;
      if(feof(d_fp))
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
  hdr = pmt_deserialize_str(str);
  delete [] hdr_buffer;

  uint64_t seg_start, extra_len;
  pmt_t r, dump;
  if(pmt_dict_has_key(hdr, pmt_string_to_symbol("strt"))) {
    r = pmt_dict_ref(hdr, pmt_string_to_symbol("strt"), dump);
    seg_start = pmt_to_uint64(r);
    extra_len = seg_start - METADATA_HEADER_SIZE;
  }

  if(extra_len > 0) {
    size = 0;
    hdr_buffer = new char[extra_len];
    while(size < extra_len) {
      ret = fread(&hdr_buffer[size], sizeof(char), extra_len-size, d_fp);
      if(ret == 0) {
	delete [] hdr_buffer;
	if(feof(d_fp))
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
    extras = pmt_deserialize_str(str);
    delete [] hdr_buffer;
  }

  return true;
}

void
gr_file_meta_source::parse_header(pmt_t hdr)
{
  // GET SAMPLE RATE
  if(pmt_dict_has_key(hdr, pmt_string_to_symbol("rx_rate"))) {
    d_samp_rate = pmt_to_double(pmt_dict_ref(hdr, pmt_string_to_symbol("rx_rate"), PMT_NIL));
  }
  else {
    throw std::runtime_error("file_meta_source: Could not extract sample rate.\n");
  }

  // GET TIME STAMP
  if(pmt_dict_has_key(hdr, pmt_string_to_symbol("rx_time"))) {
    d_time_stamp = pmt_dict_ref(hdr, pmt_string_to_symbol("rx_time"), PMT_NIL);
  }
  else {
    throw std::runtime_error("file_meta_source: Could not extract time stamp.\n");
  }

  // GET DATA TYPE
  if(pmt_dict_has_key(hdr, pmt_string_to_symbol("type"))) {
    int t = pmt_to_long(pmt_dict_ref(hdr, pmt_string_to_symbol("type"), PMT_NIL));
    switch(t) {
    case(GR_FILE_CHAR): d_itemsize = sizeof(char); break;
    case(GR_FILE_SHORT): d_itemsize = sizeof(short); break;
    case(GR_FILE_INT): d_itemsize = sizeof(int); break;
    case(GR_FILE_LONG): d_itemsize = sizeof(long int); break;
    case(GR_FILE_LONG_LONG): d_itemsize = sizeof(long long int); break;
    case(GR_FILE_FLOAT): d_itemsize = sizeof(float); break;
    case(GR_FILE_DOUBLE): d_itemsize = sizeof(double); break;
    default:
      throw std::runtime_error("file_meta_source: Could not determine data type size.\n");
    }
  }
  else {
    throw std::runtime_error("file_meta_source: Could not extract data type.\n");
  }

  // GET COMPLEX INDICATOR
  if(pmt_dict_has_key(hdr, pmt_string_to_symbol("cplx"))) {
    bool cplx = pmt_to_bool(pmt_dict_ref(hdr, pmt_string_to_symbol("cplx"), PMT_NIL));
    if(cplx)
      d_itemsize *= 2;
  }
  else {
    throw std::runtime_error("file_meta_source: Could not extract complex indicator.\n");
  }

  // GET FIRST SEGMENT SIZE
  if(pmt_dict_has_key(hdr, pmt_string_to_symbol("size"))) {
    d_seg_size = pmt_to_uint64(pmt_dict_ref(hdr, pmt_string_to_symbol("size"), PMT_NIL));
  }
  else {
    throw std::runtime_error("file_meta_source: Could not extract segment size.\n");
  }
}

bool
gr_file_meta_source::open(const char *filename)
{
  bool ret = true;
  if(d_state == STATE_DETACHED) {
    std::stringstream s;
    s << filename << ".hdr";
    ret = _open(&d_new_hdr_fp, s.str().c_str());
  }

  ret = ret && _open(&d_new_fp, filename);
  d_updated = true;
  return ret;
}

bool
gr_file_meta_source::_open(FILE **fp, const char *filename)
{
  gruel::scoped_lock guard(d_mutex); // hold mutex for duration of this function

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
gr_file_meta_source::close()
{
  gruel::scoped_lock guard(d_mutex); // hold mutex for duration of this function
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
}

void
gr_file_meta_source::do_update()
{
  if(d_updated) {
    gruel::scoped_lock guard(d_mutex); // hold mutex for duration of this block
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
gr_file_meta_source::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
{
  // We've reached the end of a segment; parse the next header and get
  // the new tags to send and set the next segment size.
  if((d_seg_size == 0) && (!feof(d_fp))) {
    pmt_t hdr=PMT_NIL, extras=PMT_NIL;
    if(read_header(hdr, extras))
      parse_header(hdr);
    else
      return -1;
  }

  char *out = (char*)output_items[0];
  int i;
  int seg_size = std::min(noutput_items, (int)d_seg_size);
  int size = seg_size;

  do_update();       // update d_fp is reqd
  if(d_fp == NULL)
    throw std::runtime_error("work with file not open");

  gruel::scoped_lock lock(d_mutex); // hold for the rest of this function
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
    if(size == seg_size)		// we didn't read anything; say we're done
      return -1;
    return seg_size - size;	// else return partial result
  }

  return seg_size;
}
