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
#ifndef INCLUDED_GR_CHECK_COUNTING_S_H
#define INCLUDED_GR_CHECK_COUNTING_S_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_check_counting_s;
typedef boost::shared_ptr<gr_check_counting_s> gr_check_counting_s_sptr;

GR_CORE_API gr_check_counting_s_sptr gr_make_check_counting_s (bool do_32bit=false);

/*!
 * \brief sink that checks if its input stream consists of a counting sequence.
 * \param do_32bit  expect an interleaved 32 bit counter in stead of 16 bit counter (default false)
 * \ingroup sink_blk
 *
 * This sink is typically used to test the USRP "Counting Mode" or "Counting mode 32 bit".
 */
class GR_CORE_API gr_check_counting_s : public gr_sync_block
{
  friend GR_CORE_API gr_check_counting_s_sptr gr_make_check_counting_s (bool do_32bit);

  enum state {
    SEARCHING,    // searching for synchronization
    LOCKED    // is locked
  };

  state      d_state;
  unsigned int    d_history;    // bitmask of decisions
  unsigned short  d_current_count;
  unsigned int    d_current_count_32bit;
  
  long      d_total_errors;
  long      d_total_shorts;
  bool      d_do_32bit;

  gr_check_counting_s (bool do_32bit);
  
  void enter_SEARCHING ();
  void enter_LOCKED ();

  void right (){
    d_history = (d_history << 1) | 0x1;
  }
  
  void wrong (){
    d_history = (d_history << 1) | 0x0;
    d_total_errors++;
  }

  bool right_three_times () { return (d_history & 0x7) == 0x7; }
  bool wrong_three_times () { return (d_history & 0x7) == 0x0; }

  void log_error (unsigned short expected, unsigned short actual);
  void log_error_32bit (unsigned int expected, unsigned int actual);
  
  int check_32bit (int noutput_items, unsigned short * in);
  int check_16bit (int noutput_items, unsigned short * in);

 public:

  int work (int noutput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_CHECK_COUNTING_S_H */
