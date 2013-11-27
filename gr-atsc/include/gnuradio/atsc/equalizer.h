/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ATSC_EQUALIZER_H
#define INCLUDED_ATSC_EQUALIZER_H

#include <gnuradio/atsc/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/atsc/consts.h>

class atsc_equalizer;
typedef boost::shared_ptr<atsc_equalizer> atsc_equalizer_sptr;

atsc_equalizer_sptr ATSC_API atsc_make_equalizer();

/*!
 * \brief ATSC equalizer (atsc_soft_data_segment --> atsc_soft_data_segment)
 * \ingroup atsc
 */
class ATSC_API atsc_equalizer : public gr::block
{
	friend ATSC_API atsc_equalizer_sptr atsc_make_equalizer();
	
public:
	atsc_equalizer();
	~atsc_equalizer () { };

	int general_work (int noutput_items,
                          gr_vector_int &ninput_items,
                          gr_vector_const_void_star &input_items,
                          gr_vector_void_star &output_items);

	void reset();

private:
	static const int NTAPS = 64;
	static const int NPRETAPS = (int) (NTAPS * 0.8); // probably should be either .2 or .8

	// the length of the field sync pattern that we know unequivocally
	static const int KNOWN_FIELD_SYNC_LENGTH = 4 + 511 + 3 * 63;

	float training_sequence1[KNOWN_FIELD_SYNC_LENGTH];
	float training_sequence2[KNOWN_FIELD_SYNC_LENGTH];

	void filterN (const float *input_samples, float *output_samples, int nsamples);
	void adaptN (const float *input_samples, const float *training_pattern,
	         float *output_samples, int nsamples);

	float d_taps[NTAPS];

	float data_mem[ATSC_DATA_SEGMENT_LENGTH + NTAPS]; // Buffer for previous data packet
	float data_mem2[ATSC_DATA_SEGMENT_LENGTH];
	unsigned short	d_flags;
	short	d_segno;

	int d_buff_not_filled;


};

#endif /* INCLUDED_ATSC_EQUALIZER_H */
