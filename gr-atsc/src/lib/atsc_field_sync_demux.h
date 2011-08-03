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
#ifndef INCLUDED_ATSC_FIELD_SYNC_DEMUX_H
#define INCLUDED_ATSC_FIELD_SYNC_DEMUX_H

#include <atsc_api.h>
#include <gr_block.h>
#include <atsc_types.h>

class atsc_field_sync_demux;
typedef boost::shared_ptr<atsc_field_sync_demux> atsc_field_sync_demux_sptr;

ATSC_API atsc_field_sync_demux_sptr atsc_make_field_sync_demux();

/*!
 * \brief ATSC Field Sync Demux
 *
 * This class accepts 1 stream of floats (data), and 1 stream of tags (syminfo). * It outputs one stream of atsc_soft_data_segment packets
 * \ingroup atsc
 *
 */
class ATSC_API atsc_field_sync_demux : public gr_block
{
  friend ATSC_API atsc_field_sync_demux_sptr atsc_make_field_sync_demux();

  atsc_field_sync_demux();

public:
  void forecast (int noutput_items, gr_vector_int &ninput_items_required);

  int  general_work (int noutput_items,
                     gr_vector_int &ninput_items,
                     gr_vector_const_void_star &input_items,
                     gr_vector_void_star &output_items);


  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void reset() { /* nop */ }

protected:
  bool            d_locked;
  bool            d_in_field2;
  int             d_segment_number;
  gr_uint64       d_next_input;
  gr_uint64       d_lost_index;         // diagnostic fluff

  unsigned long long d_inputs0_index;	// for inputs[0].index
  unsigned long	  d_inputs0_size;		// for inputs[0].size
  int		  d_consume;

};


#endif /* INCLUDED_ATSC_FIELD_SYNC_DEMUX_H */
