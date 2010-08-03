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

#include <gr_check_lfsr_32k_s.h>
#include <gr_io_signature.h>
#include <stdlib.h>
#include <stdio.h>

gr_check_lfsr_32k_s_sptr
gr_make_check_lfsr_32k_s ()
{
  return gnuradio::get_initial_sptr(new gr_check_lfsr_32k_s ());
}

gr_check_lfsr_32k_s::gr_check_lfsr_32k_s ()
  : gr_sync_block ("gr_check_lfsr_32k",
		   gr_make_io_signature (1, 1, sizeof (short)),
		   gr_make_io_signature (0, 0, 0)),
    d_state(SEARCHING), d_history (0), d_ntotal (0), d_nright (0),
    d_runlength (0), d_index(0)
{
  gri_lfsr_32k	lfsr;
  
  for (int i = 0; i < BUFSIZE; i++)
    d_buffer[i] = lfsr.next_short ();

  enter_SEARCHING ();
}

int
gr_check_lfsr_32k_s::work (int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
{
  unsigned short *in = (unsigned short *) input_items[0];

  for (int i = 0; i < noutput_items; i++){
    unsigned short	x = in[i];
    unsigned short 	expected;
    
    switch (d_state){

    case MATCH0:
      if (x == d_buffer[0])
	enter_MATCH1 ();
      break;
      
    case MATCH1:
      if (x == d_buffer[1])
	enter_MATCH2 ();
      else
	enter_MATCH0 ();
      break;
      
    case MATCH2:
      if (x == d_buffer[2])
	enter_LOCKED ();
      else
	enter_MATCH0 ();
      break;

    case LOCKED:
      expected = d_buffer[d_index];
      d_index = d_index + 1;
      if (d_index >= BUFSIZE)
	d_index = 0;

      if (x == expected)
	right ();
      else {
	wrong ();
	log_error (expected, x);
	if (wrong_three_times ())
	  enter_SEARCHING ();
      }
      break;

    default:
      abort ();
    }

    d_ntotal++;
  }

  return noutput_items;
}

void
gr_check_lfsr_32k_s::enter_SEARCHING ()
{
  d_state = SEARCHING;
  wrong ();			// reset history
  wrong ();
  wrong ();

  d_runlength = 0;
  d_index = 0;			// reset LFSR to beginning

  if (0)
    fprintf (stdout, "gr_check_lfsr_32k: enter_SEARCHING at offset %8ld (0x%08lx)\n",
	     d_ntotal, d_ntotal);

  enter_MATCH0 ();
}

void
gr_check_lfsr_32k_s::enter_MATCH0 ()
{
  d_state = MATCH0;
}

void
gr_check_lfsr_32k_s::enter_MATCH1 ()
{
  d_state = MATCH1;
}

void
gr_check_lfsr_32k_s::enter_MATCH2 ()
{
  d_state = MATCH2;
}

void
gr_check_lfsr_32k_s::enter_LOCKED ()
{
  d_state = LOCKED;
  right ();			// setup history
  right ();
  right ();

  d_index = 3;			// already matched first 3 items

  if (0)
    fprintf (stdout, "gr_check_lfsr_32k: enter_LOCKED at offset %8ld (0x%08lx)\n",
	     d_ntotal, d_ntotal);
}

void
gr_check_lfsr_32k_s::log_error (unsigned short expected, unsigned short actual)
{
  if (0)
    fprintf (stdout, 
     "gr_check_lfsr_32k: expected %5d (0x%04x) got %5d (0x%04x) offset %8ld (0x%08lx)\n",
	     expected, expected, actual, actual, d_ntotal, d_ntotal);
}
