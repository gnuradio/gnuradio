/* -*- c++ -*- */
/*
 * Copyright 2003,2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_OSCOPE_SINK_X_H
#define INCLUDED_GR_OSCOPE_SINK_X_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_trigger_mode.h>

class gr_oscope_guts;

/*!
 * \brief Abstract class for python oscilloscope module.
 * \ingroup sink_blk
 *
 * Don't instantiate this.  Use gr_oscope_sink_f or gr_oscope_sink_c instead.
 */
class GR_CORE_API gr_oscope_sink_x : public gr_sync_block
{
protected:
  double			 d_sampling_rate;
  gr_oscope_guts		*d_guts;
  
  gr_oscope_sink_x (const std::string name,
		    gr_io_signature_sptr input_sig,
		    double sampling_rate);

public:
  ~gr_oscope_sink_x ();

  bool set_update_rate (double update_rate);
  bool set_decimation_count (int decimation_count);
  bool set_trigger_channel (int channel);
  bool set_trigger_mode (gr_trigger_mode mode);
  bool set_trigger_slope (gr_trigger_slope slope);
  bool set_trigger_level (double trigger_level);
  bool set_trigger_level_auto ();				// set to 50% level
  bool set_sample_rate(double sample_rate);
  bool set_num_channels (int nchannels);


  // ACCESSORS
  int num_channels () const;
  double sample_rate () const;
  double update_rate () const;
  int get_decimation_count () const;
  int get_trigger_channel () const;
  gr_trigger_mode get_trigger_mode () const;
  gr_trigger_slope get_trigger_slope () const;
  double get_trigger_level () const;

  // # of samples written to each output record.
  int get_samples_per_output_record () const;
  
};

#endif /* INCLUDED_GR_OSCOPE_SINK_X_H */
