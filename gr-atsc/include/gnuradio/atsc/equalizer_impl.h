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

#ifndef _ATSC_EQUALIZER_H_
#define _ATSC_EQUALIZER_H_

#include <gnuradio/atsc/api.h>
#include <gnuradio/atsc/syminfo_impl.h>
#include <vector>

/*!
 * \brief abstract base class for ATSC equalizer
 */
class ATSC_API atsci_equalizer {

private:

  /*
   * have we seen a field sync since the last reset or problem?
   */
  bool	d_locked_p;

  /*
   * sample offset from the beginning of the last field sync we saw
   * to the beginning of our current input stream.  When we're locked
   * this will be in [0, 313*832] i.e., [0, 260416]
   */
  int	d_offset_from_last_field_sync;

  int	d_current_field;		// [0,1]


public:

  // CREATORS
  atsci_equalizer ();
  virtual ~atsci_equalizer ();

  virtual std::vector<double> taps () {
    return std::vector<double>();
  }

  // MANIPULATORS

  /*!
   * \brief reset state (e.g., on channel change)
   *
   * Note, subclasses must invoke the superclass's method too!
   */
  virtual void reset ();

  /*!
   * \brief produce \p nsamples of output from given inputs and tags
   *
   * This is the main entry point.  It examines the input_tags
   * and local state and invokes the appropriate virtual function
   * to handle each sub-segment of the input data.
   *
   * \p input_samples must have (nsamples + ntaps() - 1) valid entries.
   * input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] are
   * referenced to compute the output values.
   *
   * \p input_tags must have nsamples valid entries.
   * input_tags[0] .. input_tags[nsamples - 1] are referenced to
   * compute the output values.
   */
  virtual void filter (const float 	   *input_samples,
		       const atsc::syminfo *input_tags,
		       float 		   *output_samples,
		       int   		    nsamples);

  // ACCESSORS

  /*!
   * \brief how much history the input data stream requires.
   *
   * This must return a value >= 1.  Think of this as the number
   * of samples you need to look at to compute a single output sample.
   */
  virtual int ntaps () const = 0;

  /*!
   * \brief how many taps are "in the future".
   *
   * This allows us to handle what the ATSC folks call "pre-ghosts".
   * What it really does is allow the caller to jack with the offset
   * between the tags and the data so that everything magically works out.
   *
   * npretaps () must return a value between 0 and ntaps() - 1.
   *
   * If npretaps () returns 0, this means that the equalizer will only handle
   * multipath "in the past."  I suspect that a good value would be something
   * like 15% - 20% of ntaps ().
   */
  virtual int npretaps () const = 0;


protected:

  /*!
   * Input range is known NOT TO CONTAIN data segment syncs
   * or field syncs.  This should be the fast path.  In the
   * non decicion directed case, this just runs the input
   * through the filter without adapting it.
   *
   * \p input_samples has (nsamples + ntaps() - 1) valid entries.
   * input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be
   * referenced to compute the output values.
   */
  virtual void filter_normal (const float *input_samples,
			      float *output_samples,
			      int   nsamples) = 0;

  /*!
   * Input range is known to consist of only a data segment sync or a
   * portion of a data segment sync.  \p nsamples will be in [1,4].
   * \p offset will be in [0,3].  \p offset is the offset of the input
   * from the beginning of the data segment sync pattern.
   *
   * \p input_samples has (nsamples + ntaps() - 1) valid entries.
   * input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be
   * referenced to compute the output values.
   */
  virtual void filter_data_seg_sync (const float *input_samples,
				     float *output_samples,
				     int   nsamples,
				     int   offset) = 0;

  /*!
   * Input range is known to consist of only a field sync segment or a
   * portion of a field sync segment.  \p nsamples will be in [1,832].
   * \p offset will be in [0,831].  \p offset is the offset of the input
   * from the beginning of the data segment sync pattern.  We consider the
   * 4 symbols of the immediately preceding data segment sync to be the
   * first symbols of the field sync segment.  \p which_field is in [0,1]
   * and specifies which field (duh).
   *
   * \p input_samples has (nsamples + ntaps() - 1) valid entries.
   * input_samples[0] .. input_samples[nsamples - 1 + ntaps() - 1] may be
   * referenced to compute the output values.
   */
  virtual void filter_field_sync (const float *input_samples,
				  float *output_samples,
				  int   nsamples,
				  int   offset,
				  int	which_field) = 0;
};


#endif /* _ATSC_EQUALIZER_H_ */
