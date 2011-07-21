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
#ifndef INCLUDED_GR_CHECK_LFSR_32K_S_H
#define INCLUDED_GR_CHECK_LFSR_32K_S_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gri_lfsr_32k.h>


class gr_check_lfsr_32k_s;
typedef boost::shared_ptr<gr_check_lfsr_32k_s> gr_check_lfsr_32k_s_sptr;

GR_CORE_API gr_check_lfsr_32k_s_sptr gr_make_check_lfsr_32k_s ();

/*!
 * \brief sink that checks if its input stream consists of a lfsr_32k sequence.
 * \ingroup sink_blk
 *
 * This sink is typically used along with gr_lfsr_32k_source_s to test
 * the USRP using its digital loopback mode.
 */
class GR_CORE_API gr_check_lfsr_32k_s : public gr_sync_block
{
  friend GR_CORE_API gr_check_lfsr_32k_s_sptr gr_make_check_lfsr_32k_s ();

  enum state {
    SEARCHING,		// searching for synchronization
    MATCH0,
    MATCH1,
    MATCH2,
    LOCKED		// is locked
  };

  state			d_state;
  unsigned int		d_history;		// bitmask of decisions
  
  long			d_ntotal;		// total number of shorts
  long			d_nright;		// # of correct shorts
  long			d_runlength;		// # of correct shorts in a row

  static const int BUFSIZE = 2048 - 1;		// ensure pattern isn't packet aligned
  int			d_index;
  unsigned short	d_buffer[BUFSIZE];


  gr_check_lfsr_32k_s ();
  
  void enter_SEARCHING ();
  void enter_MATCH0 ();
  void enter_MATCH1 ();
  void enter_MATCH2 ();
  void enter_LOCKED ();

  void right (){
    d_history = (d_history << 1) | 0x1;
    d_nright++;
    d_runlength++;
  }
  
  void wrong (){
    d_history = (d_history << 1) | 0x0;
    d_runlength = 0;
  }

  bool right_three_times () { return (d_history & 0x7) == 0x7; }
  bool wrong_three_times () { return (d_history & 0x7) == 0x0; }

  void log_error (unsigned short expected, unsigned short actual);

 public:

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  long ntotal () const { return d_ntotal; }
  long nright () const { return d_nright; }
  long runlength () const { return d_runlength; }

};


#endif /* INCLUDED_GR_CHECK_LFSR_32K_S_H */
