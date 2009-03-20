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


GR_SWIG_BLOCK_MAGIC(gr,histo_sink_f)

gr_histo_sink_f_sptr gr_make_histo_sink_f (gr_msg_queue_sptr msgq);

class gr_histo_sink_f : public gr_sync_block
{
public:
  ~gr_histo_sink_f (void);

  unsigned int get_frame_size(void);
  unsigned int get_num_bins(void);

  void set_frame_size(unsigned int frame_size);
  void set_num_bins(unsigned int num_bins);

};
