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

#ifndef INCLUDED_GR_PWR_SQUELCH_CC_H
#define INCLUDED_GR_PWR_SQUELCH_CC_H

#include <gr_core_api.h>
#include <cmath>
#include <gr_squelch_base_cc.h>
#include <gr_single_pole_iir.h>

class gr_pwr_squelch_cc;
typedef boost::shared_ptr<gr_pwr_squelch_cc> gr_pwr_squelch_cc_sptr;

GR_CORE_API gr_pwr_squelch_cc_sptr 
gr_make_pwr_squelch_cc(double db, double alpha = 0.0001, int ramp=0, bool gate=false);

/*!
 * \brief gate or zero output when input power below threshold
 * \ingroup level_blk
 */
class GR_CORE_API gr_pwr_squelch_cc : public gr_squelch_base_cc
{
private:
  double d_threshold;
  double d_pwr;
  gr_single_pole_iir<double,double,double> d_iir;

  friend GR_CORE_API gr_pwr_squelch_cc_sptr gr_make_pwr_squelch_cc(double db, double alpha, int ramp, bool gate);
  gr_pwr_squelch_cc(double db, double alpha, int ramp, bool gate);

protected:
  virtual void update_state(const gr_complex &in);
  virtual bool mute() const { return d_pwr < d_threshold; }
  
public:
  std::vector<float> squelch_range() const;

  double threshold() const { return 10*log10(d_threshold); }
  void set_threshold(double db) { d_threshold = std::pow(10.0, db/10); }
  void set_alpha(double alpha) { d_iir.set_taps(alpha); }
};

#endif /* INCLUDED_GR_PWR_SQUELCH_CC_H */
