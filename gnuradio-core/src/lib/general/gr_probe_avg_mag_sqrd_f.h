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
#ifndef INCLUDED_GR_PROBE_AVG_MAG_SQRD_F_H
#define INCLUDED_GR_PROBE_AVG_MAG_SQRD_F_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_single_pole_iir.h>

class gr_probe_avg_mag_sqrd_f;
typedef boost::shared_ptr<gr_probe_avg_mag_sqrd_f> gr_probe_avg_mag_sqrd_f_sptr;

GR_CORE_API gr_probe_avg_mag_sqrd_f_sptr
gr_make_probe_avg_mag_sqrd_f (double threshold_db, double alpha = 0.0001);

/*!
 * \brief compute avg magnitude squared.
 * \ingroup sink_blk
 *
 * input: float
 *
 * Compute a running average of the magnitude squared of the the input.
 * The level and indication as to whether the level exceeds threshold
 * can be retrieved with the level and unmuted accessors.
 */
class GR_CORE_API gr_probe_avg_mag_sqrd_f : public gr_sync_block
{
  double					d_threshold;
  gr_single_pole_iir<double,double,double>	d_iir;
  bool						d_unmuted;
  double					d_level;

  friend GR_CORE_API gr_probe_avg_mag_sqrd_f_sptr
  gr_make_probe_avg_mag_sqrd_f (double threshold_db, double alpha);

  gr_probe_avg_mag_sqrd_f (double threshold_db, double alpha);

public:
  ~gr_probe_avg_mag_sqrd_f ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  // ACCESSORS
  bool unmuted () const { return d_unmuted; }
  double level () const { return d_level; }

  double threshold() const;

  // SETTERS
  void set_alpha (double alpha);
  void set_threshold (double decibels);
};

#endif /* INCLUDED_GR_PROBE_AVG_MAG_SQRD_F_H */
