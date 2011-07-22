/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_NOAA_HRPT_DEFRAMER_H
#define INCLUDED_NOAA_HRPT_DEFRAMER_H

#include <noaa_api.h>
#include <gr_block.h>

class noaa_hrpt_deframer;
typedef boost::shared_ptr<noaa_hrpt_deframer> noaa_hrpt_deframer_sptr;

NOAA_API noaa_hrpt_deframer_sptr
noaa_make_hrpt_deframer();

class NOAA_API noaa_hrpt_deframer : public gr_block
{
  friend NOAA_API noaa_hrpt_deframer_sptr noaa_make_hrpt_deframer();
  noaa_hrpt_deframer();

  unsigned int       d_state;
  bool               d_mid_bit;
  unsigned char      d_last_bit;
  unsigned int       d_bit_count;
  unsigned int       d_word_count;
  unsigned long long d_shifter;     // 60 bit sync word
  unsigned short     d_word;        // 10 bit HRPT word

  void enter_idle();
  void enter_synced();
 
public:
  int general_work(int noutput_items,
		   gr_vector_int &ninput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_NOAA_HRPT_DEFRAMER_H */
