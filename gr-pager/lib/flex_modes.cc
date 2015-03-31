/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2012 Free Software Foundation, Inc.
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

#include "flex_modes.h"

namespace gr {
  namespace pager {

    const flex_mode_t
    flex_modes[] =
      {
	{ (int32_t) 0x870C78F3, 1600, 2 },
	{ (int32_t) 0xB0684F97, 1600, 4 },
	{ (int32_t) 0x7B1884E7, 3200, 2 },
	{ (int32_t) 0xDEA0215F, 3200, 4 },
	{ (int32_t) 0x4C7CB383, 3200, 4 }
      };

    const int num_flex_modes = sizeof(flex_modes)/sizeof(flex_modes[0]);

    unsigned char flex_bcd[17] = "0123456789 U -][";

    const char *flex_page_desc[] =
      {
	"ENC",
	"UNK",
	"TON",
	"NUM",
	"SPN",
	"ALN",
	"BIN",
	"NNM"
      };

    int
    find_flex_mode(int32_t sync_code)
    {
      for(int i = 0; i < num_flex_modes; i++)
	if(flex_modes[i].sync == sync_code)
	  return i;

      // Not found
      return -1;
    }

  } /* namespace pager */
} /* namespace gr */
