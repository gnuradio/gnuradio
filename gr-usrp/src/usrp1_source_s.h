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

#ifndef INCLUDED_USRP1_SOURCE_S_H
#define INCLUDED_USRP1_SOURCE_S_H

#include <usrp1_source_base.h>
#include <stdexcept>

class usrp_standard_rx;


class usrp1_source_s;
typedef boost::shared_ptr<usrp1_source_s> usrp1_source_s_sptr;


// public shared_ptr constructor

usrp1_source_s_sptr
usrp1_make_source_s (int which_board, 
		     unsigned int decim_rate,
		     int nchan,
		     int mux,
		     int mode,
		     int fusb_block_size,
		     int fusb_nblocks,
		     const std::string fpga_filename,
		     const std::string firmware_filename
		     ) throw (std::runtime_error);

/*!
 * \brief interface to Universal Software Radio Peripheral Rx path (Rev 1)
 *
 * output: 1 stream of short
 */
class usrp1_source_s : public usrp1_source_base {
 private:
  friend usrp1_source_s_sptr
  usrp1_make_source_s (int which_board, 
		       unsigned int decim_rate,
		       int nchan,
		       int mux,
		       int mode,
		       int fusb_block_size,
		       int fusb_nblocks,
		       const std::string fpga_filename,
		       const std::string firmware_filename
		       ) throw (std::runtime_error);

 protected:
  usrp1_source_s (int which_board, 
		  unsigned int decim_rate,
		  int nchan,
		  int mux,
		  int mode,
		  int fusb_block_size,
		  int fusb_nblocks,
		  const std::string fpga_filename,
		  const std::string firmware_filename
		  ) throw (std::runtime_error);

  virtual int ninput_bytes_reqd_for_noutput_items (int noutput_items);

  virtual void copy_from_usrp_buffer (gr_vector_void_star &output_items,
				      int output_index,
				      int output_items_available,
				      int &output_items_produced,
				      const void *usrp_buffer,
				      int usrp_buffer_length,
				      int &bytes_read);
 public:
  ~usrp1_source_s ();
};

#endif /* INCLUDED_USRP1_SOURCE_S_H */
