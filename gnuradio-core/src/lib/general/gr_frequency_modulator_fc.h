/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FREQUENCY_MODULATOR_FC_H
#define INCLUDED_GR_FREQUENCY_MODULATOR_FC_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_frequency_modulator_fc;
typedef boost::shared_ptr<gr_frequency_modulator_fc> gr_frequency_modulator_fc_sptr;

GR_CORE_API gr_frequency_modulator_fc_sptr gr_make_frequency_modulator_fc (double sensitivity);

/*!
 * \brief Frequency modulator block
 * \ingroup modulation_blk
 *
 * float input; complex baseband output
 */
class GR_CORE_API gr_frequency_modulator_fc : public gr_sync_block
{
  float	d_sensitivity;
  float	d_phase;

  friend GR_CORE_API gr_frequency_modulator_fc_sptr
  gr_make_frequency_modulator_fc (double sensitivity);

  gr_frequency_modulator_fc (double sensitivity);

 public:
  void set_sensitivity(float sens) { d_sensitivity = sens; }
  float sensitivity() const { return d_sensitivity; }
  
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_FREQUENCY_MODULATOR_FC_H */
