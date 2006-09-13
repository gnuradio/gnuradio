/* -*- c++ -*- */
/*
 * Copyright 2004,2005 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

class gr_io_signature;
typedef boost::shared_ptr<gr_io_signature>  gr_io_signature_sptr;
%template(gr_io_signature_sptr) boost::shared_ptr<gr_io_signature>;

%rename(io_signature) gr_make_io_signature;

gr_io_signature_sptr
gr_make_io_signature (int min_streams,
		      int max_streams,
		      size_t sizeof_stream_item);

class gr_io_signature {
 public:

  // disabled. Suspected bug in SWIG 1.3.24
  // static const int IO_INFINITE = -1;
    
  ~gr_io_signature ();
    
  int min_streams () const { return d_min_streams; }
  int max_streams () const { return d_max_streams; }
  size_t sizeof_stream_item (int index) const { return d_sizeof_stream_item; }

 private:
  gr_io_signature (int min_streams, int max_streams, size_t sizeof_stream_item);
};

