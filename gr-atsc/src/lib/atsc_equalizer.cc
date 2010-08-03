/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <atsc_equalizer.h>
#include <create_atsci_equalizer.h>
#include <gr_io_signature.h>
#include <atsc_consts.h>
#include <atsci_syminfo.h>


atsc_equalizer_sptr
atsc_make_equalizer()
{
  return gnuradio::get_initial_sptr(new atsc_equalizer());
}

// had atsc_equalizer(atsci_equalizer *equalizer)
atsc_equalizer::atsc_equalizer()
  : gr_sync_block("atsc_equalizer",
		  gr_make_io_signature(2, 2, sizeof(float)),
		  gr_make_io_signature(2, 2, sizeof(float)))
{
  d_equalizer = create_atsci_equalizer_lms();
}

atsc_equalizer::~atsc_equalizer ()
{
  // Anything that isn't automatically cleaned up...

  delete d_equalizer;
}


void
atsc_equalizer::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{

  int ntaps = d_equalizer->ntaps ();

  unsigned ninputs = ninput_items_required.size();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = fixed_rate_noutput_to_ninput (noutput_items + ntaps);


}



int
atsc_equalizer::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  const atsc::syminfo *in_tags = (const atsc::syminfo *) input_items[1];
  float *out = (float *) output_items[0];
  atsc::syminfo *out_tags = (atsc::syminfo *) output_items[1];

  assert(sizeof(float) == sizeof(atsc::syminfo));

  int ntaps = d_equalizer->ntaps ();
  int npretaps = d_equalizer->npretaps ();

  assert (ntaps >= 1);
  assert (npretaps >= 0 && npretaps < ntaps);

  int offset = ntaps - npretaps - 1;
  assert (offset >= 0 && offset < ntaps);


  // peform the actual equalization

  d_equalizer->filter (in, in_tags + offset,
                       out, noutput_items);

  // write the output tags

  for (int i = 0; i < noutput_items; i++)
    out_tags[i] = in_tags[i + offset];

  return noutput_items;
}
