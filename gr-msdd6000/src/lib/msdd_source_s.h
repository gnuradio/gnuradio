/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_MSDD_SOURCE_S_H
#define INCLUDED_MSDD_SOURCE_S_H

#include <msdd_source_base.h>
#include <msdd_buffer_copy_behaviors.h>
#include <stdexcept>

class msdd_source_s;
typedef boost::shared_ptr<msdd_source_s> msdd_source_s_sptr;

// public shared_ptr constructor

msdd_source_s_sptr
msdd_make_source_s (int which_board, 
		    unsigned int decim_rate,
		    unsigned int fft_points,
		    double initial_rx_freq,
		    int opp_mode,
		    const char *src, 
		    unsigned short port_src
		    ) throw (std::runtime_error);

/*!
 * \brief interface to MSDD Rx path
 *
 * output: 1 stream of short
 */
class msdd_source_s : public msdd_source_base {
private:
  friend msdd_source_s_sptr
  msdd_make_source_s (int which_board, 
		      unsigned int decim_rate,
		      unsigned int fft_points,
		      double initial_rx_freq,
		      int opp_mode,
		      const char *src, 
		      unsigned short port_src
		      ) throw (std::runtime_error);

  std::auto_ptr<msdd::BufferCopyBehavior> d_buffer_copy_behavior;
  
protected:
  msdd_source_s (int which_board, 
		 unsigned int decim_rate,
		 unsigned int fft_points,
		 double initial_rx_freq,
		 int opp_mode,
		 const char *src, 
		 unsigned short port_src
		 ) throw (std::runtime_error);

  int ninput_bytes_reqd_for_noutput_items (int noutput_items);

  virtual void copy_from_msdd_buffer (gr_vector_void_star &output_items,
				      int output_index,
				      int output_items_available,
				      int &output_items_produced,
				      const void *msdd_buffer,
				      int buffer_length,
				      int &bytes_read);
public:
  ~msdd_source_s ();
};

#endif /* INCLUDED_MSDD_SOURCE_S_H */
