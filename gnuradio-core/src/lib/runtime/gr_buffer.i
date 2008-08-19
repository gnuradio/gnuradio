/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

class gr_buffer;
typedef boost::shared_ptr<gr_buffer> gr_buffer_sptr;
%template(gr_buffer_sptr) boost::shared_ptr<gr_buffer>;
%rename(buffer) gr_make_buffer;
%ignore gr_buffer;

gr_buffer_sptr gr_make_buffer (int nitems, size_t sizeof_item, gr_block_sptr link);

class gr_buffer {
 public:
  ~gr_buffer ();

 private:
  gr_buffer (int nitems, size_t sizeof_item, gr_block_sptr link);
};
  

class gr_buffer_reader;
typedef boost::shared_ptr<gr_buffer_reader> gr_buffer_reader_sptr;
%template(gr_buffer_reader_sptr) boost::shared_ptr<gr_buffer_reader>;
%ignore gr_buffer_reader;

%rename(buffer_add_reader) gr_buffer_add_reader;
gr_buffer_reader_sptr gr_buffer_add_reader (gr_buffer_sptr buf, int nzero_preload, gr_block_sptr link);

class gr_buffer_reader {
 public:
  ~gr_buffer_reader ();

 private:
  friend class gr_buffer;
  gr_buffer_reader (gr_buffer_sptr buffer, unsigned int read_index, gr_block_sptr link);
};


%rename(buffer_ncurrently_allocated) gr_buffer_ncurrently_allocated;
long gr_buffer_ncurrently_allocated ();

%rename(buffer_reader_ncurrently_allocated) gr_buffer_reader_ncurrently_allocated;
long gr_buffer_reader_ncurrently_allocated ();

