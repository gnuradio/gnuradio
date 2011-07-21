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
#ifndef INCLUDED_GR_VCO_F_H
#define INCLUDED_GR_VCO_F_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_fxpt_vco.h>

/*!
 * \brief VCO - Voltage controlled oscillator
 * \ingroup misc
 *
 * \param sampling_rate		sampling rate (Hz)
 * \param sensitivity		units are radians/sec/volt
 * \param amplitude		output amplitude
 */
class gr_vco_f;
typedef boost::shared_ptr<gr_vco_f> gr_vco_f_sptr;


GR_CORE_API gr_vco_f_sptr gr_make_vco_f(double sampling_rate, double sensitivity, double amplitude);

/*!
 * \brief VCO - Voltage controlled oscillator
 * \ingroup modulator_blk
 *
 * input: float stream of control voltages; output: float oscillator output
 */
class GR_CORE_API gr_vco_f : public gr_sync_block
{
  friend GR_CORE_API gr_vco_f_sptr gr_make_vco_f(double sampling_rate, double sensitivity, double amplitude);

  /*!
   * \brief VCO - Voltage controlled oscillator
   *
   * \param sampling_rate	sampling rate (Hz)
   * \param sensitivity		units are radians/sec/volt
   * \param amplitude		output amplitude
   */
  gr_vco_f(double sampling_rate, double sensitivity, double amplitude);

  double	d_sampling_rate;
  double	d_sensitivity;
  double	d_amplitude;
  double	d_k;
  gr_fxpt_vco	d_vco;

public:

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_VCO_F_H */
