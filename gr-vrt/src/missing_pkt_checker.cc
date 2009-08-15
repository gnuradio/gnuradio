/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <missing_pkt_checker.h>

int
missing_pkt_checker::check(const vrt::expanded_header *hdr)
{
  // FIXME assumes we're inspecting only a single stream

  int nmissing = 0;
  int actual = hdr->pkt_cnt();
  int expected = (d_last_pkt_cnt + 1) & 0xf;
  if (actual != expected && !d_resync){
    d_nwrong_pkt_cnt++;
    if (actual > expected)
      nmissing = actual - expected;
    else
      nmissing = actual + 16 - expected;
    d_nmissing_pkt_est += nmissing;
  }
  d_last_pkt_cnt = actual;
  d_npackets++;
  d_resync = false;
  return nmissing;
}
