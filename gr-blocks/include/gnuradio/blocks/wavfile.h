/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

// This file stores all the RIFF file type knowledge for the wavfile_*
// gnuradio/blocks.

#ifndef _GR_WAVFILE_H_
#define _GR_WAVFILE_H_

#include <gnuradio/blocks/api.h>
#include <cstdio>

namespace gr {
namespace blocks {


//! WAV file header information.
struct wav_header_info {
    // TODO: refactor to use correct types (int16/32, etc.).

    //! sample rate [S/s]
    unsigned sample_rate;

    //! Number of channels
    int nchans;

    //! Bytes per sample
    /** Can either be 1 or 2 (corresponding to 8 or 16 bit samples, respectively) */
    int bytes_per_sample;

    //! Number of the first byte containing a sample
    /** Use this with fseek() to jump from the end of the file to the
     *  first sample when in repeat mode.
     */
    int first_sample_pos;

    //! Number of samples per channel
    unsigned samples_per_chan;

    //! Size of DATA chunk
    unsigned data_chunk_size;
};

/*!
 * \brief Read signal information from a given WAV file.
 *
 * \param[in]  fp     File pointer to an opened, empty file.
 * \param[out] info   Parsed information.
 * \return True on a successful read, false if the file could not be read or is
 *         not a valid or incompatible WAV file.
 */
BLOCKS_API bool wavheader_parse(FILE* fp, wav_header_info& info);

/*!
 * \brief Read one sample from an open WAV file at the current position.
 *
 * \details
 * Takes care of endianness.
 */
BLOCKS_API short int wav_read_sample(FILE* fp, int bytes_per_sample);


/*!
 * \brief Write a valid RIFF file header
 *
 * Note: Some header values are kept blank because they're usually
 * not known a-priori (file and chunk lengths). Use
 * gri_wavheader_complete() to fill these in.
 */
BLOCKS_API bool
wavheader_write(FILE* fp, unsigned int sample_rate, int nchans, int bytes_per_sample);

/*!
 * \brief Write one sample to an open WAV file at the current position.
 *
 * \details
 * Takes care of endianness.
 */
BLOCKS_API void wav_write_sample(FILE* fp, short int sample, int bytes_per_sample);


/*!
 * \brief Complete a WAV header
 *
 * \details
 * Note: The stream position is changed during this function. If
 * anything needs to be written to the WAV file after calling this
 * function (which shouldn't happen), you need to fseek() to the
 * end of the file (or wherever).
 *
 * \param[in] fp               File pointer to an open WAV file with a blank header
 * \param[in] first_sample_pos Position of the first sample in DATA chunk.
 */
BLOCKS_API bool wavheader_complete(FILE* fp, unsigned first_sample_pos);

} /* namespace blocks */
} /* namespace gr */

#endif /* _GR_WAVFILE_H_ */
