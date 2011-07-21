/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_ALIGN_ON_SAMPLE_NUMBERS_SS_H
#define INCLUDED_GR_ALIGN_ON_SAMPLE_NUMBERS_SS_H

#include <gr_core_api.h>
#include <gr_block.h>

/*!
 * \brief align several complex short (interleaved short) input channels with corresponding unsigned 32 bit sample_counters (provided as interleaved 16 bit values)
 * \ingroup misc
 * \param number of complex_short input channels (including the 32 bit counting channel)
 * \param align_interval is after how much samples (minimally) the sample-alignement is refreshed. Default is 128.
 * A bigger value means less processing power but also requests more buffer space, which has a maximum.
 * Decrease the align_interval if you get an error like:
 * "sched: <gr_block align_on_samplenumbers_ss (0)> is requesting more input data  than we can provide.
 *  ninput_items_required = 32768
 *  max_possible_items_available = 16383
 *  If this is a filter, consider reducing the number of taps."
 * \ingroup block
 * Pay attention on how you connect this block.
 * It expects a minimum of 2 usrp_source_s with nchan number of channels and FPGA_MODE_COUNTING_32BIT enabled.
 * This means that the first complex_short channel on every input is an interleaved 32 bit counter. 
 * The samples are aligned by dropping samples untill the samplenumbers match.
 */
class gr_align_on_samplenumbers_ss;
typedef boost::shared_ptr<gr_align_on_samplenumbers_ss> gr_align_on_samplenumbers_ss_sptr;

GR_CORE_API gr_align_on_samplenumbers_ss_sptr gr_make_align_on_samplenumbers_ss (int nchan=2, int align_interval=128);




class GR_CORE_API gr_align_on_samplenumbers_ss : public gr_block
{
  int  d_align_interval;
  int  d_sample_counter;
  int  d_nchan;
  bool d_in_presync;
  unsigned int d_ninputs;
  class GR_CORE_API align_state {
      public:
        unsigned int ucounter_end;
        unsigned int ucounter_begin;
        int diff;
        int diff_comp;
        int diff_end;
        int diff_comp_end;
        bool sync_found;
        bool sync_end_found;
        int ninput_items;
        int ninput_items_used;
      };
  std::vector<align_state>   d_state;
    
  friend GR_CORE_API gr_align_on_samplenumbers_ss_sptr gr_make_align_on_samplenumbers_ss (int nchan,int align_interval);
  gr_align_on_samplenumbers_ss (int nchan,int align_interval);

 public:
   ~gr_align_on_samplenumbers_ss();
  bool check_topology (int ninputs, int noutputs);
  void forecast (int noutput_items,
        gr_vector_int &ninput_items_required);

  int general_work (int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_ALIGN_ON_SAMPLE_NUMBERS_SS_H */
