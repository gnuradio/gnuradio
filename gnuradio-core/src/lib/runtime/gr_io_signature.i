/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2007 Free Software Foundation, Inc.
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

class gr_io_signature;
typedef boost::shared_ptr<gr_io_signature>  gr_io_signature_sptr;
%template(gr_io_signature_sptr) boost::shared_ptr<gr_io_signature>;

%rename(io_signature)  gr_make_io_signature;
%rename(io_signature2) gr_make_io_signature2;
%rename(io_signature3) gr_make_io_signature3;
%rename(io_signaturev) gr_make_io_signaturev;


gr_io_signature_sptr
gr_make_io_signature(int min_streams, int max_streams,
		     int sizeof_stream_item);

gr_io_signature_sptr
gr_make_io_signature2(int min_streams, int max_streams,
		      int sizeof_stream_item1,
		      int sizeof_stream_item2
		      );
gr_io_signature_sptr
gr_make_io_signature3(int min_streams, int max_streams, 
		      int sizeof_stream_item1,
		      int sizeof_stream_item2,
		      int sizeof_stream_item3
		      );
gr_io_signature_sptr
gr_make_io_signaturev(int min_streams, int max_streams,
		      const std::vector<int> &sizeof_stream_items);


class gr_io_signature {
  gr_io_signature (int min_streams, int max_streams, int sizeof_stream_item);

  friend gr_io_signature_sptr 
  gr_make_io_signaturev(int min_streams,
			int max_streams,
			const std::vector<int> &sizeof_stream_item);

 public:

  // disabled. Suspected bug in SWIG 1.3.24
  // static const int IO_INFINITE = -1;
    
  ~gr_io_signature ();
    
  int min_streams () const { return d_min_streams; }
  int max_streams () const { return d_max_streams; }
  int sizeof_stream_item (int index) const;
  std::vector<int> sizeof_stream_items() const;
};

