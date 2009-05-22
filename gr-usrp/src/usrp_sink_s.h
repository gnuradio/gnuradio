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

#ifndef INCLUDED_USRP_SINK_S_H
#define INCLUDED_USRP_SINK_S_H

#include <usrp_sink_base.h>

class usrp_sink_s;
typedef boost::shared_ptr<usrp_sink_s> usrp_sink_s_sptr;


// public shared_ptr constructor

usrp_sink_s_sptr
usrp_make_sink_s (int which_board=0,
		  unsigned int interp_rate=32,
		  int nchan=1,
		  int mux=-1,
		  int fusb_block_size=0,
		  int fusb_nblocks=0,
		  const std::string fpga_filename="",
		  const std::string firmware_filename=""
		  ) throw (std::runtime_error);

/*!
 * \brief Interface to Universal Software Radio Peripheral Tx path
 * \ingroup usrp
 * \ingroup sink_blk
 *
 * input: short
 */
class usrp_sink_s : public usrp_sink_base {
 private:

  friend usrp_sink_s_sptr
  usrp_make_sink_s (int which_board,
		     unsigned int interp_rate,
		     int nchan,
		     int mux,
		     int fusb_block_size,
		     int fusb_nblocks,
		     const std::string fpga_filename,
		     const std::string firmware_filename
		     ) throw (std::runtime_error);

 protected:
  usrp_sink_s (int which_board,
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
  ~usrp_sink_s ();
};

#endif /* INCLUDED_USRP_SINK_S_H */
