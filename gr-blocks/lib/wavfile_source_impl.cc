/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2010,2013,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <algorithm>
#include <cstring>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "wavfile_source_impl.h"
#include <gnuradio/io_signature.h>
#include <sys/types.h>
#include <stdexcept>

namespace gr {
namespace blocks {

wavfile_source::sptr wavfile_source::make(const char* filename, bool repeat)
{
    return gnuradio::make_block_sptr<wavfile_source_impl>(filename, repeat);
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
    SF_INFO sfinfo;

    sfinfo.format = 0;
    errno = 0;
    if (!(d_fp = sf_open(filename, SFM_READ, &sfinfo))) {
        if (errno) {
            d_logger->error("sf_open failed: {:s}: {:s}", filename, strerror(errno));
        } else {
            d_logger->error("sf_open failed: {:s}: {:s}", filename, sf_strerror(NULL));
        }
        throw std::runtime_error("Can't open WAV file.");
    }

    d_h.sample_rate = (unsigned)sfinfo.samplerate;
    d_h.nchans = sfinfo.channels;
    if (sfinfo.channels > s_max_channels) {
        throw std::runtime_error("Number of channels greater than " +
                                 std::to_string(s_max_channels) + " not supported.");
    }

    switch (sfinfo.format & SF_FORMAT_SUBMASK) {
    case SF_FORMAT_PCM_S8:
        d_h.bytes_per_sample = 1;
        break;
    case SF_FORMAT_PCM_16:
        d_h.bytes_per_sample = 2;
        break;
    case SF_FORMAT_PCM_24:
        d_h.bytes_per_sample = 3;
        break;
    case SF_FORMAT_PCM_32:
        d_h.bytes_per_sample = 4;
        break;
    case SF_FORMAT_PCM_U8:
        d_h.bytes_per_sample = 1;
        break;
    case SF_FORMAT_FLOAT:
        d_h.bytes_per_sample = 4;
        break;
    case SF_FORMAT_DOUBLE:
        d_h.bytes_per_sample = 8;
        break;
    }
    d_h.samples_per_chan = sfinfo.frames;

    if (d_h.samples_per_chan == 0) {
        throw std::runtime_error("WAV file does not contain any samples.");
    }

    // can't set_output_multiple, because we don't know whether the file is actually a
    // multiple of that in length!
    // set_output_multiple(s_items_size);
    d_buffer.resize(s_samps_per_read * d_h.nchans);

    // Re-set the output signature
    set_output_signature(io_signature::make(1, d_h.nchans, sizeof(float)));
}

wavfile_source_impl::~wavfile_source_impl() { sf_close(d_fp); }

int wavfile_source_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    auto out = (float**)&output_items[0];
    auto n_out_chans = output_items.size();
    int produced = 0;

    for (int i = 0; i < noutput_items; i += s_samps_per_read) {
        if (d_sample_idx >= d_h.samples_per_chan) {
            if (!d_repeat) {
                return produced ? produced : -1;
            }

            if (sf_seek(d_fp, 0, SEEK_SET) == -1) {
                d_logger->error("sf_seek failed: {:s}", strerror(errno));
                throw std::runtime_error("Seek error.");
            }

            d_sample_idx = 0;
        }

        const auto requested_samples =
            std::min(d_h.nchans * s_samps_per_read,
                     d_h.nchans * static_cast<decltype(s_samps_per_read)>(noutput_items));
        const auto read_samples = sf_read_float(d_fp, d_buffer.data(), requested_samples);
        const auto items = read_samples / d_h.nchans;
        for (int n = 0; n < items; n++) {
            for (int chan = 0; chan < d_h.nchans; chan++) {
                if (static_cast<size_t>(chan) < n_out_chans) {
                    out[chan][n + produced] = d_buffer[chan + (n * d_h.nchans)];
                }
            }
        }

        produced += items;
        d_sample_idx += items;

        // We're not going to deal with handling corrupt wav files,
        // so if they give us any trouble they won't be processed.
        // Serves them bloody right.

        auto errnum = sf_error(d_fp);
        if (errnum) {
            if (items == 0) {
                d_logger->error("WAV file has corrupted header or I/O error, {:s}",
                                sf_error_number(errnum));
                return -1;
            }
            d_logger->error(
                "Encountered libsndfile error {:d} ({:s}), trying to continue");
            return produced;
        }
        if (read_samples < requested_samples) {
            // this was the end of the file!
            break;
        }
    }

    return produced ? produced : -1;
}

} /* namespace blocks */
} /* namespace gr */
