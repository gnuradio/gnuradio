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

#ifndef INCLUDED_GR_TRIGGER_MODE_H
#define INCLUDED_GR_TRIGGER_MODE_H

enum gr_trigger_mode {
  gr_TRIG_MODE_FREE,
  gr_TRIG_MODE_AUTO,
  gr_TRIG_MODE_NORM,
  gr_TRIG_MODE_STRIPCHART,
};

enum gr_trigger_slope {
  gr_TRIG_SLOPE_POS,
  gr_TRIG_SLOPE_NEG,
};

#endif /* INCLUDED_GR_TRIGGER_MODE_H */
