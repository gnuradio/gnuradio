/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2005 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_OSCOPE_SINK_F_H
#define INCLUDED_GR_OSCOPE_SINK_F_H

#include <gr_core_api.h>
#include <gr_oscope_sink_x.h>
#include <gr_msg_queue.h>

class gr_oscope_sink_f;
typedef boost::shared_ptr<gr_oscope_sink_x> gr_oscope_sink_f_sptr;

GR_CORE_API gr_oscope_sink_f_sptr gr_make_oscope_sink_f (double sampling_rate, gr_msg_queue_sptr msgq);


/*!
 * \brief Building block for python oscilloscope module.
 * \ingroup sink_blk
 *
 * Accepts multiple float streams.
 */
class GR_CORE_API gr_oscope_sink_f : public gr_oscope_sink_x
{
private:
  friend GR_CORE_API gr_oscope_sink_f_sptr
  gr_make_oscope_sink_f (double sampling_rate, gr_msg_queue_sptr msgq);

  gr_oscope_sink_f (double sampling_rate, gr_msg_queue_sptr msgq);

  gr_msg_queue_sptr	d_msgq;

 public:
  ~gr_oscope_sink_f ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  bool check_topology (int ninputs, int noutputs);
};

#endif /* INCLUDED_GR_OSCOPE_SINK_F_H */

