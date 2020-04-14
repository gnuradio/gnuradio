/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010,2013,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "wavfile_source_impl.h"
#include <gnuradio/blocks/wavfile.h>
#include <gnuradio/io_signature.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdexcept>

// win32 (mingw/msvc) specific
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef O_BINARY
#define OUR_O_BINARY O_BINARY
#else
#define OUR_O_BINARY 0
#endif
// should be handled via configure
#ifdef O_LARGEFILE
#define OUR_O_LARGEFILE O_LARGEFILE
#else
#define OUR_O_LARGEFILE 0
#endif

namespace gr {
namespace blocks {

wavfile_source::sptr wavfile_source::make(const char* filename, bool repeat)
{
    return gnuradio::get_initial_sptr(new wavfile_source_impl(filename, repeat));
}

wavfile_source_impl::wavfile_source_impl(const char* filename, bool repeat)
    : sync_block("wavfile_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 2, sizeof(float))),
      d_fp(NULL),
      d_repeat(repeat),
      d_h{}, // Init with zeros
      d_sample_idx(0)
{
    // we use "open" to use to the O_LARGEFILE flag
    int fd;
    if ((fd = ::open(filename, O_RDONLY | OUR_O_LARGEFILE | OUR_O_BINARY)) < 0) {
        GR_LOG_ERROR(d_logger,
                     boost::format("::open: %s: %s") % filename % strerror(errno));
        throw std::runtime_error("can't open file");
    }

    if ((d_fp = fdopen(fd, "rb")) == NULL) {
        GR_LOG_ERROR(d_logger,
                     boost::format("fdopen: %s: %s") % filename % strerror(errno));
        throw std::runtime_error("can't open file");
    }

    // Scan headers, check file validity
    if (!wavheader_parse(d_fp, d_h)) {
        throw std::runtime_error("is not a valid wav file");
    }


    if (d_h.samples_per_chan == 0) {
        throw std::runtime_error("WAV file does not contain any samples");
    }

    if (d_h.bytes_per_sample == 1) {
        d_normalize_fac = UINT8_MAX / 2;
        d_normalize_shift = 1;
    } else {
        d_normalize_fac = INT16_MAX;
        d_normalize_shift = 0;
    }

    // Re-set the output signature
    set_output_signature(io_signature::make(1, d_h.nchans, sizeof(float)));
}

wavfile_source_impl::~wavfile_source_impl() { fclose(d_fp); }

float wavfile_source_impl::convert_to_float(short int sample)
{
    float sample_out = (float)sample;
    sample_out /= d_normalize_fac;
    sample_out -= d_normalize_shift;
    return sample_out;
}

int wavfile_source_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    auto out = (float**)&output_items[0];
    int n_out_chans = output_items.size();

    int i;
    short sample;

    for (i = 0; i < noutput_items; i++) {
        if (d_sample_idx >= d_h.samples_per_chan) {
            if (!d_repeat) {
                // if nothing was read at all, say we're done.
                return i ? i : -1;
            }

            if (fseek(d_fp, d_h.first_sample_pos, SEEK_SET) == -1) {
                GR_LOG_ERROR(d_logger,
                             boost::format("fseek failed %s") % strerror(errno));
                exit(-1);
            }

            d_sample_idx = 0;
        }

        for (int chan = 0; chan < d_h.nchans; chan++) {
            sample = wav_read_sample(d_fp, d_h.bytes_per_sample);

            if (chan < n_out_chans) {
                out[chan][i] = convert_to_float(sample);
            }
        }

        d_sample_idx++;

        // OK, EOF is not necessarily an error. But we're not going to
        // deal with handling corrupt wav files, so if they give us any
        // trouble they won't be processed. Serves them bloody right.
        if (feof(d_fp) || ferror(d_fp)) {
            if (i == 0) {
                GR_LOG_ERROR(d_logger,
                             boost::format("WAV file has corrupted header or i/o error") %
                                 strerror(errno));
                return -1;
            }
            return i;
        }
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
