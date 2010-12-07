/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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

%include <gr_basic_block.i>

class gr_block;
typedef boost::shared_ptr<gr_block> gr_block_sptr;
%template(gr_block_sptr) boost::shared_ptr<gr_block>;

// support vectors of these...
namespace std {
  %template(x_vector_gr_block_sptr)	vector<gr_block_sptr>;
};

class gr_block : public gr_basic_block {
 protected:
  gr_block (const std::string &name,
            gr_io_signature_sptr input_signature,
            gr_io_signature_sptr output_signature);

 public:
  
  virtual ~gr_block ();
  
  unsigned history () const;

  int  output_multiple () const;
  double relative_rate () const;

  bool start();
  bool stop();

  uint64_t nitems_read(unsigned int which_input);
  uint64_t nitems_written(unsigned int which_output);

  // internal use
  gr_block_detail_sptr detail () const { return d_detail; }
  void set_detail (gr_block_detail_sptr detail) { d_detail = detail; }
};
