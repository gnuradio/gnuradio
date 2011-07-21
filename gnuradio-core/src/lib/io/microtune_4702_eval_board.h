/* -*- C++ -*- */
/*
 * Copyright 2001,2004 Free Software Foundation, Inc.
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

#ifndef INCLUDED_MICROTUNE_4702_EVAL_BOARD_H
#define INCLUDED_MICROTUNE_4702_EVAL_BOARD_H

#include <gr_core_api.h>
#include "microtune_xxxx_eval_board.h"

/*!
 * \brief control microtune 4702 eval board
 * \ingroup hardware
 */

class GR_CORE_API microtune_4702_eval_board : public microtune_xxxx_eval_board {
public:
  microtune_4702_eval_board (int which_pp = 0);
  ~microtune_4702_eval_board ();

  /*!
   * \brief set RF and IF AGC levels together (scale [0, 1000])
   *
   * This provides a simple linear interface for adjusting both
   * the RF and IF gain in consort.  This is the easy to use interface.
   * 0 corresponds to minimum gain. 1000 corresponds to maximum gain.
   */
  virtual void set_AGC (float value_0_1000);
};

#endif /* INCLUDED_MICROTUNE_4702_EVAL_BOARD_H */
