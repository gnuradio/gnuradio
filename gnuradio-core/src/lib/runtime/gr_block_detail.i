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

class gr_block_detail;
typedef boost::shared_ptr<gr_block_detail> gr_block_detail_sptr;
%template(gr_block_detail_sptr) boost::shared_ptr<gr_block_detail>;
%rename(block_detail) gr_make_block_detail;
%ignore gr_block_detail;

gr_block_detail_sptr gr_make_block_detail (unsigned int ninputs, unsigned int noutputs);

class gr_block_detail {
 public:

  ~gr_block_detail ();

  int ninputs () const { return d_ninputs; }
  int noutputs () const { return d_noutputs; }
  bool sink_p () const { return d_noutputs == 0; }
  bool source_p () const { return d_ninputs == 0; }

  void set_input (unsigned int which, gr_buffer_reader_sptr reader);
  gr_buffer_reader_sptr input (unsigned int which)
  {
    if (which >= d_ninputs)
      throw std::invalid_argument ("gr_block_detail::input");
    return d_input[which];
  }

  void set_output (unsigned int which, gr_buffer_sptr buffer);
  gr_buffer_sptr output (unsigned int which)
  {
    if (which >= d_noutputs)
      throw std::invalid_argument ("gr_block_detail::output");
    return d_output[which];
  }

  // ----------------------------------------------------------------------------

 private:
  gr_block_detail (unsigned int ninputs, unsigned int noutputs);

};


%rename(block_detail_ncurrently_allocated) gr_block_detail_ncurrently_allocated;
long gr_block_detail_ncurrently_allocated ();
