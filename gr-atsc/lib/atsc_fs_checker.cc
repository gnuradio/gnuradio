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

#include <gnuradio/atsc/fs_checker.h>
#include <gnuradio/atsc/create_atsci_fs_checker.h>
#include <gnuradio/atsc/fs_checker_impl.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/atsc/consts.h>
#include <gnuradio/atsc/syminfo_impl.h>


atsc_fs_checker_sptr
atsc_make_fs_checker()
{
  return gnuradio::get_initial_sptr(new atsc_fs_checker());
}

atsc_fs_checker::atsc_fs_checker()
  : gr::sync_block("atsc_fs_checker",
		  gr::io_signature::make(2, 2, sizeof(float)),
		  gr::io_signature::make(2, 2, sizeof(float)))
{
  d_fsc = create_atsci_fs_checker();
}


atsc_fs_checker::~atsc_fs_checker ()
{
  // Anything that isn't automatically cleaned up...

  delete d_fsc;
}


int
atsc_fs_checker::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  const float *in = (const float *) input_items[0];
  const atsc::syminfo *tag_in = (const atsc::syminfo *) input_items[1];
  float *out = (float *) output_items[0];
  atsc::syminfo *tag_out = (atsc::syminfo *) output_items[1];

  assert(sizeof(float) == sizeof(atsc::syminfo));


  for (int i = 0; i < noutput_items; i++)
    d_fsc->filter (in[i], tag_in[i], &out[i], &tag_out[i]);

  return noutput_items;
}
