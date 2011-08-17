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

#ifndef _ATSC_EQUALIZER_LMS_H_
#define _ATSC_EQUALIZER_LMS_H_

#include <atsc_api.h>
#include <atsci_equalizer.h>
#include <vector>
#include <stdio.h>

class ATSC_API atsci_equalizer_lms : public atsci_equalizer
{
public:
  atsci_equalizer_lms ();
  virtual ~atsci_equalizer_lms ();

  virtual void reset ();
  virtual int ntaps () const;
  virtual int npretaps () const;
  
protected:
  FILE *trainingfile;
  virtual void filter_normal (const float *input_samples,
			      float *output_samples,
			      int   nsamples);

  virtual void filter_data_seg_sync (const float *input_samples,
				     float *output_samples,
				     int   nsamples,
				     int   offset);
  
  virtual void filter_field_sync (const float *input_samples,
				  float *output_samples,
				  int   nsamples,
				  int   offset,
				  int	which_field);

private:
  std::vector<double>	d_taps;

  void filterN (const float *input_samples,
		float *output_samples,
		int nsamples);

  void adaptN (const float *input_samples,
	       const float *training_pattern,
	       float *output_samples,
	       int    nsamples);

  float filter1 (const float input[]);
  float adapt1 (const float input[], float ideal_output);

};


#endif /* _ATSC_EQUALIZER_LMS_H_ */
