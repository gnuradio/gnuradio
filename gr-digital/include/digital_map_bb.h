/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_MAP_BB_H
#define INCLUDED_GR_MAP_BB_H

#include <digital_api.h>
#include <gr_sync_block.h>

class digital_map_bb;
typedef boost::shared_ptr<digital_map_bb> digital_map_bb_sptr;

DIGITAL_API digital_map_bb_sptr
digital_make_map_bb(const std::vector<int> &map);

/*!
 * \brief output[i] = map[input[i]]
 * \ingroup coding_blk
 *
 * This block maps an incoming signal to the value in the map.
 * The block expects that the incoming signal has a maximum
 * value of len(map)-1.
 *
 * -> output[i] = map[input[i]]
 *
 * \param map a vector of integers.
 */

class DIGITAL_API digital_map_bb : public gr_sync_block
{
  friend DIGITAL_API digital_map_bb_sptr
    digital_make_map_bb(const std::vector<int> &map);

  unsigned char d_map[0x100];

  digital_map_bb(const std::vector<int> &map);

public:
  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_MAP_BB_H */
