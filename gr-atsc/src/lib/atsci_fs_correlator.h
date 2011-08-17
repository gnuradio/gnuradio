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
#ifndef _ATSC_FS_CORRELATOR_H_
#define _ATSC_FS_CORRELATOR_H_

#include <atsc_api.h>

/*!
 * \brief abstract base class for ATSC field sync correlator
 *
 * Processes input samples one at a time looking for
 * an occurence of either the field sync 1 or field sync 2 pattern.
 *
 * For each sample processed, an output sample and an output tag are produced.
 * The output samples are identical to the input samples but are delayed by
 * a number of samples given by \p delay().  The output tag associated with
 * the the given output sample indicates whether this sample is the beginning
 * of one of the field syncs or is an ordinary sample.  The tags are defined in
 * atsci_sync_tag.h.
 *
 * For ease of use, the field sync patterns are defined to begin with the
 * first symbol of the 4 symbol data segment sync pattern that immediately
 * proceeds the actual PN 511 code.  This makes it easier for downstream code
 * to determine the location of data segment syncs merely by counting.  They'll
 * occur every 832 samples assuming everything is working.
 */

class ATSC_API atsci_fs_correlator {

public:

  // CREATORS
  atsci_fs_correlator ();
  virtual ~atsci_fs_correlator () = 0;

  // MANIPULATORS
  virtual void reset () = 0;
  virtual void filter (float input_sample, float *output_sample, float *output_tag) = 0;

  // ACCESSORS

  //! return delay in samples from input to output
  virtual int delay () const = 0;
};
 
#endif /* _ATSC_FS_CORRELATOR_H_ */
