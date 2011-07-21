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

#ifndef INCLUDED_GR_CTCSS_SQUELCH_FF_H
#define INCLUDED_GR_CTCSS_SQUELCH_FF_H

#include <gr_core_api.h>
#include <gr_squelch_base_ff.h>
#include <gri_goertzel.h>

class gr_ctcss_squelch_ff;
typedef boost::shared_ptr<gr_ctcss_squelch_ff> gr_ctcss_squelch_ff_sptr;

GR_CORE_API gr_ctcss_squelch_ff_sptr 
gr_make_ctcss_squelch_ff(int rate, float freq, float level=0.01, int len=0, int ramp=0, bool gate=false);

/*!
 * \brief gate or zero output if ctcss tone not present
 * \ingroup level_blk
 */
class GR_CORE_API gr_ctcss_squelch_ff : public gr_squelch_base_ff
{
private:
  float d_freq;
  float d_level;
  int   d_len;
  bool  d_mute;
     
  gri_goertzel d_goertzel_l;
  gri_goertzel d_goertzel_c;
  gri_goertzel d_goertzel_r;

  friend GR_CORE_API gr_ctcss_squelch_ff_sptr gr_make_ctcss_squelch_ff(int rate, float freq, float level, int len, int ramp, bool gate);
  gr_ctcss_squelch_ff(int rate, float freq, float level, int len, int ramp, bool gate);

  int find_tone(float freq);

protected:
  virtual void update_state(const float &in);
  virtual bool mute() const { return d_mute; }
  
public:
  std::vector<float> squelch_range() const;
  float level() const { return d_level; }
  void set_level(float level) { d_level = level; }
  int len() const { return d_len; }
};

#endif /* INCLUDED_GR_CTCSS_SQUELCH_FF_H */
