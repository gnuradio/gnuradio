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

#ifndef INCLUDED_USRP1_SINK_C_H
#define INCLUDED_USRP1_SINK_C_H

#include <usrp1_sink_base.h>

class usrp1_sink_c;
typedef boost::shared_ptr<usrp1_sink_c> usrp1_sink_c_sptr;


// public shared_ptr constructor

usrp1_sink_c_sptr
usrp1_make_sink_c (int which_board,
		   unsigned int interp_rate,
		   int nchan,
		   int mux,
		   int fusb_block_size,
		   int fusb_nblocks,
		   const std::string fpga_filename,
		   const std::string firmware_filename
		   ) throw (std::runtime_error);


/*!
 * \brief interface to Universal Software Radio Peripheral Tx path (Rev 1)
 *
 * input: gr_complex
 */
class usrp1_sink_c : public usrp1_sink_base {
 private:

  friend usrp1_sink_c_sptr
  usrp1_make_sink_c (int which_board,
		     unsigned int interp_rate,
		     int nchan,
		     int mux,
		     int fusb_block_size,
		     int fusb_nblocks,
		     const std::string fpga_filename,
		     const std::string firmware_filename
		     ) throw (std::runtime_error);

 protected:
  usrp1_sink_c (int which_board,
		unsigned int interp_rate,
		int nchan,
		int mux,
		int fusb_block_size,
		int fusb_nblocks,
		const std::string fpga_filename,
		const std::string firmware_filename
		) throw (std::runtime_error);

  virtual void copy_to_usrp_buffer (gr_vector_const_void_star &input_items,
				    int  input_index,
				    int	 input_items_available,
				    int  &input_items_consumed,
				    void *usrp_buffer,
				    int  usrp_buffer_length,
				    int	 &bytes_written);
 public:
  ~usrp1_sink_c ();
};

#endif /* INCLUDED_USRP1_SINK_C_H */
