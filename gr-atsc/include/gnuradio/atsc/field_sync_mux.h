/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ATSC_FIELD_SYNC_MUX_H
#define INCLUDED_ATSC_FIELD_SYNC_MUX_H

#include <gnuradio/atsc/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/atsc/types.h>

class atsc_field_sync_mux;
typedef boost::shared_ptr<atsc_field_sync_mux> atsc_field_sync_mux_sptr;

ATSC_API atsc_field_sync_mux_sptr atsc_make_field_sync_mux();

/*!
 * \brief Insert ATSC Field Syncs as required (atsc_data_segment --> atsc_data_segment)
 * \ingroup atsc
 *
 * input: atsc_data_segment; output: atsc_data_segment
 */
class ATSC_API atsc_field_sync_mux : public gr::block
{
  friend ATSC_API atsc_field_sync_mux_sptr atsc_make_field_sync_mux();

  atsc_field_sync_mux();

public:
  void forecast (int noutput_items, gr_vector_int &ninput_items_required);
  int general_work (int noutput_items,
	    gr_vector_int &ninput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);


  static const int      N_SAVED_SYMBOLS = 12;

  void reset()
  {
    d_already_output_field_sync = false;
  }

protected:
  bool                  d_already_output_field_sync;
  unsigned char         d_saved_symbols[N_SAVED_SYMBOLS];
};


#endif /* INCLUDED_ATSC_FIELD_SYNC_MUX_H */
