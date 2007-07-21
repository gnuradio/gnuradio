/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#include <math.h>
#include <atsci_fake_single_viterbi.h>
#include <iostream>
#include <algorithm>

using std::cerr;
using std::cout;

void
atsci_fake_single_viterbi::reset()
{
  post_coder_state = 0;
}

atsci_fake_single_viterbi::atsci_fake_single_viterbi()
{
  reset();
}

/*
 * implement simple slicer and post coder
 */
char
atsci_fake_single_viterbi::decode (float input)
{
  int	y2, y1;
  
  if (input < -4){
    y2 = 0;
    y1 = 0;
  }
  else if (input < 0){
    y2 = 0;
    y1 = 1;
  }
  else if (input < 4){
    y2 = 1;
    y1 = 0;
  }
  else {
    y2 = 1;
    y1 = 1;
  }

  int	x1 = y1;
  int	x2 = y2 ^ post_coder_state;
  post_coder_state = y2;

  return (x2 << 1) | x1;
}
