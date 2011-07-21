/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_gr_peak_detector2_FB_H
#define INCLUDED_gr_peak_detector2_FB_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_peak_detector2_fb;
typedef boost::shared_ptr<gr_peak_detector2_fb> gr_peak_detector2_fb_sptr;

GR_CORE_API gr_peak_detector2_fb_sptr gr_make_peak_detector2_fb (float threshold_factor_rise = 7,
						     int look_ahead = 1000,
						     float alpha = 0.001);

/*!
 * \brief Detect the peak of a signal
 * \ingroup level_blk
 *
 * If a peak is detected, this block outputs a 1, 
 * or it outputs 0's.  A separate debug output may be connected, to
 * view the internal EWMA described below.
 *
 * \param threshold_factor_rise The threshold factor determins when a peak
 *        is present. An EWMA average of the signal is calculated and when the 
 *        value of the signal goes over threshold_factor_rise*average, we
 *        call the peak.
 * \param look_ahead The look-ahead value is used when the threshold is
 *        found to locate the peak within this range.
 * \param alpha The gain value of a single-pole moving average filter
 */

class GR_CORE_API gr_peak_detector2_fb : public gr_sync_block
{
  friend GR_CORE_API gr_peak_detector2_fb_sptr 
  gr_make_peak_detector2_fb (float threshold_factor_rise, int look_ahead, float alpha);
  
  gr_peak_detector2_fb (float threshold_factor_rise, int look_ahead, float alpha);
  
private:
  float d_threshold_factor_rise;
  int d_look_ahead;
  int d_look_ahead_remaining;
  int d_peak_ind;
  float d_peak_val;
  float d_alpha;
  float d_avg;
  bool d_found;
  
public:
  
  /*! \brief Set the threshold factor value for the rise time
   *  \param thr new threshold factor
   */
  void set_threshold_factor_rise(float thr) { d_threshold_factor_rise = thr; }
  
  /*! \brief Set the look-ahead factor
   *  \param look new look-ahead factor
   */
  void set_look_ahead(int look) { d_look_ahead = look; }
  
  /*! \brief Set the running average alpha
   *  \param alpha new alpha for running average
   */
  void set_alpha(int alpha) { d_alpha = alpha; }
  
  /*! \brief Get the threshold factor value for the rise time
   *  \return threshold factor
   */
  float threshold_factor_rise() { return d_threshold_factor_rise; }
  
  /*! \brief Get the look-ahead factor value
   *  \return look-ahead factor
   */
  int look_ahead() { return d_look_ahead; }
  
  /*! \brief Get the alpha value of the running average
   *  \return alpha
   */
  float alpha() { return d_alpha; }
  
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif


