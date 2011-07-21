/* -*- c++ -*- */
/*
 * Copyright 2004,2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_WAVFILE_SOURCE_H
#define INCLUDED_GR_WAVFILE_SOURCE_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <cstdio>  // for FILE

class gr_wavfile_source;
typedef boost::shared_ptr<gr_wavfile_source> gr_wavfile_source_sptr;

GR_CORE_API gr_wavfile_source_sptr
gr_make_wavfile_source (const char *filename, bool repeat = false);

/*!
 * \brief Read stream from a Microsoft PCM (.wav) file, output floats
 *
 * Unless otherwise called, values are within [-1;1].
 * Check gr_make_wavfile_source() for extra info.
 *
 * \ingroup source_blk
 */

class GR_CORE_API gr_wavfile_source : public gr_sync_block
{
private:
  friend GR_CORE_API gr_wavfile_source_sptr gr_make_wavfile_source (const char *filename,
		                                              bool repeat);
  gr_wavfile_source(const char *filename, bool repeat);

  FILE *d_fp;
  bool d_repeat;
  
  unsigned d_sample_rate;
  int d_nchans;
  int d_bytes_per_sample;
  int d_first_sample_pos;
  unsigned d_samples_per_chan;
  unsigned d_sample_idx;
  int d_normalize_shift;
  int d_normalize_fac;
  
  /*!
   * \brief Convert an integer sample value to a float value within [-1;1]
   */
  float convert_to_float(short int sample);

public:
  ~gr_wavfile_source ();
  
  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

  /*!
   * \brief Read the sample rate as specified in the wav file header
   */
  unsigned int sample_rate() const { return d_sample_rate; };

  /*!
   * \brief Return the number of bits per sample as specified in the wav
   * file header. Only 8 or 16 bit are supported here.
   */
  int bits_per_sample() const { return d_bytes_per_sample * 8; };
  
  /*!
   * \brief Return the number of channels in the wav file as specified in
   * the wav file header. This is also the max number of outputs you can
   * have.
   */
  int channels() const { return d_nchans; };
};

#endif /* INCLUDED_GR_WAVFILE_SOURCE_H */
