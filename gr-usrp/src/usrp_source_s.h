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

#ifndef INCLUDED_USRP_SOURCE_S_H
#define INCLUDED_USRP_SOURCE_S_H

#include <usrp_source_base.h>
#include <stdexcept>

class usrp_standard_rx;


class usrp_source_s;
typedef boost::shared_ptr<usrp_source_s> usrp_source_s_sptr;


// public shared_ptr constructor

usrp_source_s_sptr
usrp_make_source_s (int which_board=0, 
		    unsigned int decim_rate=16,
		    int nchan=1,
		    int mux=-1,
		    int mode=0,
		    int fusb_block_size=0,
		    int fusb_nblocks=0,
		    const std::string fpga_filename="",
		    const std::string firmware_filename=""
		    ) throw (std::runtime_error);

/*!
 * \brief Interface to Universal Software Radio Peripheral Rx path
 * \ingroup usrp
 * \ingroup source_blk
 *
 * output: 1 stream of short
 */
class usrp_source_s : public usrp_source_base {
 private:
  friend usrp_source_s_sptr
  usrp_make_source_s (int which_board, 
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
  usrp_source_s (int which_board, 
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
  ~usrp_source_s ();
};

#endif /* INCLUDED_USRP_SOURCE_S_H */
