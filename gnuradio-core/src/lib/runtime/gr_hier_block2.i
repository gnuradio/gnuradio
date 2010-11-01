/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2007 Free Software Foundation, Inc.
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

class gr_hier_block2;
typedef boost::shared_ptr<gr_hier_block2> gr_hier_block2_sptr;
%template(gr_hier_block2_sptr) boost::shared_ptr<gr_hier_block2>;

// Hack to have a Python shim implementation of gr.hier_block2
// that instantiates one of these and passes through calls
%rename(hier_block2_swig) gr_make_hier_block2;
gr_hier_block2_sptr gr_make_hier_block2(const std::string name,
                                        gr_io_signature_sptr input_signature,
                                        gr_io_signature_sptr output_signature)
  throw (std::runtime_error);

// Rename connect and disconnect so that we can more easily build a
// better interface in scripting land.
%rename(primitive_connect) gr_hier_block2::connect;
%rename(primitive_disconnect) gr_hier_block2::disconnect;

class gr_hier_block2 : public gr_basic_block
{
private:
  gr_hier_block2(const std::string name,
		 gr_io_signature_sptr input_signature,
		 gr_io_signature_sptr output_signature);
  
public:
  ~gr_hier_block2 ();
  
  void connect(gr_basic_block_sptr block)
    throw (std::invalid_argument);
  void connect(gr_basic_block_sptr src, int src_port,
	       gr_basic_block_sptr dst, int dst_port)
    throw (std::invalid_argument);
  void disconnect(gr_basic_block_sptr block)
    throw (std::invalid_argument);
  void disconnect(gr_basic_block_sptr src, int src_port,
		  gr_basic_block_sptr dst, int dst_port)
    throw (std::invalid_argument);
  void disconnect_all();
  void lock();
  void unlock();

  gr_hier_block2_sptr to_hier_block2(); // Needed for Python/Guile type coercion
};
