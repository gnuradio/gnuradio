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

GR_SWIG_BLOCK_MAGIC(gr,regenerate_bb)

gr_regenerate_bb_sptr gr_make_regenerate_bb (int period, unsigned int max_regen=500);

class gr_regenerate_bb : public gr_sync_block
{
 private:
  gr_regenerate_bb (int period, unsigned int max_regen);

public:
  void set_max_regen(unsigned int regen);
  
  /*! \brief Reset the period of regenerations; this will reset the current regen.
   */
  void set_period(int period);
};
