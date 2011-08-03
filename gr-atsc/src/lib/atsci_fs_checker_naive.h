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

#ifndef _ATSC_FS_CHECKER_NAIVE_H_
#define _ATSC_FS_CHECKER_NAIVE_H_

#include <atsc_api.h>
#include <atsci_fs_checker.h>

/*!
 * \brief Naive concrete implementation of field sync checker
 */
class ATSC_API atsci_fs_checker_naive : public atsci_fs_checker {

 private:
  static const int	SRSIZE = 1024;		// must be power of two
  int			d_index;		// points at oldest sample
  float			d_sample_sr[SRSIZE];	// sample shift register
  atsc::syminfo		d_tag_sr[SRSIZE];	// tag shift register
  unsigned char		d_bit_sr[SRSIZE];	// binary decision shift register
  int			d_field_num;
  int			d_segment_num;

  static const int	OFFSET_511 = 0;		// offset to PN 511 pattern
  static const int	LENGTH_511 = 511 + 4;	// length of PN 511 pattern (+ 4 seg sync)
  static const int	OFFSET_2ND_63 = 578;	// offset to second PN 63 pattern
  static const int	LENGTH_2ND_63 = 63;	// length of PN 63 pattern

  static unsigned char	s_511[LENGTH_511];	// PN 511 pattern
  static unsigned char	s_63[LENGTH_2ND_63];	// PN 63 pattern

  inline static int wrap (int index){ return index & (SRSIZE - 1); }
  inline static int incr (int index){ return wrap (index + 1); }
  inline static int decr (int index){ return wrap (index - 1); }

 public:

  // CREATORS
  atsci_fs_checker_naive ();
  ~atsci_fs_checker_naive ();

  // MANIPULATORS
  virtual void reset ();
  void filter (float input_sample, atsc::syminfo input_tag,
	       float *output_sample, atsc::syminfo *output_tag);

  // ACCESSORS

  //! return delay in samples from input to output
  int delay () const;
  
};


#endif /* _ATSC_FS_CHECKER_NAIVE_H_ */
