/* -*- c++ -*- */
/*
 * Copyright 2005,2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

GR_SWIG_BLOCK_MAGIC(gr,agc_ff)

%include <gri_agc.i>

gr_agc_ff_sptr
gr_make_agc_ff (float rate = 1e-4, float reference = 1.0, float gain = 1.0, float max_gain = 0.0);

class gr_agc_ff : public gr_sync_block , public gri_agc
{
  gr_agc_ff (float rate, float reference, float gain, float max_gain);
};
