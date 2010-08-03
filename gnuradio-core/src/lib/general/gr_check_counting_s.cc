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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_check_counting_s.h>
#include <gr_io_signature.h>
#include <stdlib.h>
#include <stdio.h>

gr_check_counting_s_sptr
gr_make_check_counting_s (bool do_32bit)
{
  return gnuradio::get_initial_sptr(new gr_check_counting_s (do_32bit));
}

gr_check_counting_s::gr_check_counting_s (bool do_32bit)
  : gr_sync_block ("gr_check_counting",
    gr_make_io_signature (1, 1, sizeof (short)),
    gr_make_io_signature (0, 0, 0)),
    d_state(SEARCHING), d_history (0), d_current_count (0), d_current_count_32bit(0),
    d_total_errors (0), d_total_shorts (0),
    d_do_32bit(do_32bit)
{
  enter_SEARCHING ();
}

int
gr_check_counting_s::work (int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
  unsigned short *in = (unsigned short *) input_items[0];
  if(d_do_32bit)
    return check_32bit(noutput_items,in);
  else
    return check_16bit(noutput_items,in);
}

int
gr_check_counting_s::check_16bit (int noutput_items,
        unsigned short * in)
{
  for (int i = 0; i < noutput_items; i++){
    unsigned short  x = in[i];
    
    switch (d_state){

    case SEARCHING:
      if (x == d_current_count){
       right ();
       log_error (d_current_count, x);
       d_current_count = d_current_count + 1;
       if (right_three_times ())
         enter_LOCKED ();
      }
      else {
        wrong ();
        log_error (d_current_count, x);
        d_current_count = x + 1;
      }
      break;

    case LOCKED:
      if (x == d_current_count){
        right ();
        d_current_count = d_current_count + 1;
      }
      else {
        wrong ();
        log_error (d_current_count, x);
        d_current_count = d_current_count + 1;
        if (wrong_three_times ())
          enter_SEARCHING ();
      }
      break;

    default:
      abort ();
    }

    d_total_shorts++;
  }

  return noutput_items;
}

int
gr_check_counting_s::check_32bit (int noutput_items,
        unsigned short * in)
{

  for (int i = 0; i < noutput_items-1; i+=2){
    unsigned int  x_high16bits = in[i];
    unsigned int  x_low16bits = in[i+1];
    unsigned int  x = x_high16bits<<16 | x_low16bits;
    
    switch (d_state){

    case SEARCHING:
      if (x == d_current_count_32bit){
        right ();
        log_error_32bit (d_current_count_32bit, x);
        d_current_count_32bit = d_current_count_32bit + 1;
        if (right_three_times ())
          enter_LOCKED ();
      }
      else {
        wrong ();
        log_error_32bit (d_current_count_32bit, x);
        d_current_count_32bit = x + 1;
      }
      break;

    case LOCKED:
      if (x == d_current_count_32bit){
        right ();
        d_current_count_32bit = d_current_count_32bit + 1;
      }
      else {
        wrong ();
        log_error_32bit (d_current_count_32bit, x);
        d_current_count_32bit = d_current_count_32bit + 1;
        if (wrong_three_times ())
          enter_SEARCHING ();
      }
      break;

    default:
      abort ();
    }

    d_total_shorts++;
  }

  return noutput_items;
}

void
gr_check_counting_s::enter_SEARCHING ()
{
  d_state = SEARCHING;
  fprintf (stdout, "gr_check_counting: enter_SEARCHING at offset %8ld (0x%08lx)\n",
     d_total_shorts, d_total_shorts);
}

void
gr_check_counting_s::enter_LOCKED ()
{
  d_state = LOCKED;
  fprintf (stdout, "gr_check_counting: enter_LOCKED at offset %8ld (0x%08lx)\n",
     d_total_shorts, d_total_shorts);
}

void
gr_check_counting_s::log_error (unsigned short expected, unsigned short actual)
{
  fprintf (stdout, 
"gr_check_counting: expected %5d (0x%04x) got %5d (0x%04x) offset %8ld (0x%08lx)\n",
     expected, expected, actual, actual, d_total_shorts, d_total_shorts);
}

void
gr_check_counting_s::log_error_32bit (unsigned int expected, unsigned int actual)
{
  fprintf (stdout, 
"gr_check_counting: expected %10d (0x%08x) got %10d (0x%08x) offset %8ld (0x%08lx)\n",
     expected, expected, actual, actual, d_total_shorts, d_total_shorts);
}
