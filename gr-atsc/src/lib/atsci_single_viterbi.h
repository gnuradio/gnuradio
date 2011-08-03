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

#ifndef _ATSCSINGLEVITERBI_H_
#define _ATSCSINGLEVITERBI_H_

#include <atsc_api.h>

/*!
 * \brief single channel viterbi decoder
 */
class ATSC_API atsci_single_viterbi
{
  
public:
  atsci_single_viterbi ();

  static const unsigned int TB_LEN = 32;

  /*!
   * \p INPUT ideally takes on the values +/- 1,3,5,7
   * return is decoded dibit in the range [0, 3]
   */
  char decode (float input);

  void reset ();

  //! internal delay of decoder
  int delay () { return TB_LEN - 1; }

protected:
  static const int transition_table[32];
  static const float was_sent[32];
  float path_metrics [2][8];
  unsigned long long traceback [2][8];
  unsigned char phase;
};

#endif 
