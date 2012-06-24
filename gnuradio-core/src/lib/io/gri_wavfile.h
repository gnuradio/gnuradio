/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

// This file stores all the RIFF file type knowledge for the gr_wavfile_*
// blocks.

#include <gr_core_api.h>
#include <cstdio>

/*!
 * \brief Read signal information from a given WAV file.
 *
 * \param[in]  fp          File pointer to an opened, empty file.
 * \param[out] sample_rate Stores the sample rate [S/s]
 * \param[out] nchans      Number of channels
 * \param[out] bytes_per_sample Bytes per sample, can either be 1 or 2 (corresponding o
 *                              8 or 16 bit samples, respectively)
 * \param[out] first_sample_pos Number of the first byte containing a sample. Use this
 *                              with fseek() to jump from the end of the file to the
 *                              first sample when in repeat mode.
 * \param[out] samples_per_chan Number of samples per channel
 * \return True on a successful read, false if the file could not be read or is
 *         not a valid WAV file.
 */
bool
gri_wavheader_parse(FILE *fp,
		    unsigned int &sample_rate,
		    int &nchans,
		    int &bytes_per_sample,
		    int &first_sample_pos,
		    unsigned int &samples_per_chan);


/*!
 * \brief Read one sample from an open WAV file at the current position.
 *
 * Takes care of endianness.
 */
short int
gri_wav_read_sample(FILE *fp, int bytes_per_sample);


/*!
 * \brief Write a valid RIFF file header
 *
 * Note: Some header values are kept blank because they're usually not known
 * a-priori (file and chunk lengths). Use gri_wavheader_complete() to fill
 * these in.
 */
bool
gri_wavheader_write(FILE *fp,
		 unsigned int sample_rate,
		 int nchans,
		 int bytes_per_sample);

/*!
 * \brief Write one sample to an open WAV file at the current position.
 *
 * Takes care of endianness.
 */
void
gri_wav_write_sample(FILE *fp, short int sample, int bytes_per_sample);


/*!
 * \brief Complete a WAV header
 *
 * Note: The stream position is changed during this function. If anything
 * needs to be written to the WAV file after calling this function (which
 * shouldn't happen), you  need to fseek() to the end of the file (or
 * whereever).
 *
 * \param[in] fp         File pointer to an open WAV file with a blank header
 * \param[in] byte_count Length of all samples written to the file in bytes.
 */
bool
gri_wavheader_complete(FILE *fp, unsigned int byte_count);
