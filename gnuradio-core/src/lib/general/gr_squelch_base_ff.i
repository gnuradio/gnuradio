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

#include <gr_block.h>

class gr_squelch_base_ff : public gr_block
{
private:
  enum { ST_MUTED, ST_ATTACK, ST_UNMUTED, ST_DECAY } d_state;

public:
  gr_squelch_base_ff(const char *name, int ramp, bool gate);

  int ramp() const { return d_ramp; }
  void set_ramp(int ramp) { d_ramp = ramp; }
  bool gate() const { return d_gate; }
  void set_gate(bool gate) { d_gate = gate; }
  bool unmuted() const { return (d_state == ST_UNMUTED || d_state == ST_ATTACK); }

  virtual std::vector<float> squelch_range() const = 0;
};
