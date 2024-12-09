/* -*- c++ -*- */
/*
 * Copyright 2004,2006-2011,2013,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "wavfile_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include <cstring>
#include <stdexcept>

namespace gr {
namespace blocks {

wavfile_sink::sptr wavfile_sink::make(const char* filename,
                                      int n_channels,
                                      unsigned int sample_rate,
                                      wavfile_format_t format,
                                      wavfile_subformat_t subformat,
                                      bool append)
{
    return gnuradio::make_block_sptr<wavfile_sink_impl>(
        filename, n_channels, sample_rate, format, subformat, append);
}

wavfile_sink_impl::wavfile_sink_impl(const char* filename,
                                     int n_channels,
                                     unsigned int sample_rate,
                                     wavfile_format_t format,
                                     wavfile_subformat_t subformat,
                                     bool append)
    : sync_block("wavfile_sink",
                 io_signature::make(1, n_channels, sizeof(float)),
                 io_signature::make(0, 0, 0)),
      d_h{}, // Init with zeros
      d_append(append),
      d_fp(nullptr),
      d_new_fp(nullptr),
      d_updated(false)
{
    int bits_per_sample{ 8 };

    if (n_channels > s_max_channels) {
        throw std::runtime_error("Number of channels greater than " +
                                 std::to_string(s_max_channels) + " not supported.");
    }

    d_h.sample_rate = sample_rate;
    d_h.nchans = n_channels;
    d_h.format = format;
    d_h.subformat = subformat;
    switch (subformat) {
    case FORMAT_PCM_S8:
        bits_per_sample = 8;
        break;
    case FORMAT_PCM_16:
        bits_per_sample = 16;
        break;
    case FORMAT_PCM_24:
        bits_per_sample = 24;
        break;
    case FORMAT_PCM_32:
        bits_per_sample = 32;
        break;
    case FORMAT_PCM_U8:
        bits_per_sample = 8;
        break;
    case FORMAT_FLOAT:
        bits_per_sample = 32;
        break;
    case FORMAT_DOUBLE:
        bits_per_sample = 64;
        break;
    case FORMAT_VORBIS:
        bits_per_sample = 32;
        break;
    case FORMAT_OPUS:
        bits_per_sample = 32;
        break;
    }
    d_h.bytes_per_sample = bits_per_sample / 8;

    set_max_noutput_items(s_items_size);
    d_buffer.resize(s_items_size * d_h.nchans);

    if (!open(filename)) {
        throw std::runtime_error("Can't open WAV file.");
    }
}

bool wavfile_sink_impl::open(const char* filename)
{
    gr::thread::scoped_lock guard(d_mutex);
    d_filename = filename;

    if (d_new_fp) { // if we've already got a new one open, close it
        sf_close(d_new_fp);
        d_new_fp = nullptr;
    }

    bool isOpen = d_append ? open_file_for_append() : open_file_for_rewrite();
    return isOpen;
}

bool wavfile_sink_impl::open_file_for_append()
{
    SF_INFO sfinfo;

    // We are appending to an existing file, be extra careful here.
    sfinfo.format = 0;
    errno = 0;
    if (!(d_new_fp = sf_open(d_filename.c_str(), SFM_RDWR, &sfinfo))) {
        if (errno) {
            d_logger->error("sf_open(1) failed: {:s}: {:s}", d_filename, strerror(errno));
        } else {
            d_logger->error(
                "sf_open(1) failed: {:s}: {:s}", d_filename, sf_strerror(NULL));
        }

        return false;
    }
    if (d_h.sample_rate != sfinfo.samplerate || d_h.nchans != sfinfo.channels ||
        d_h.format != (sfinfo.format & SF_FORMAT_TYPEMASK) ||
        d_h.subformat != (sfinfo.format & SF_FORMAT_SUBMASK)) {
        d_logger->error("Existing WAV file is incompatible with configured options.");
        sf_close(d_new_fp);
        return false;
    }
    if (sf_seek(d_new_fp, 0, SEEK_END) == -1) {
        d_logger->error("Seek error.");
        return false; // This can only happen if the file disappears under our feet.
    }
    d_updated = true;
    return true;
}

bool wavfile_sink_impl::open_file_for_rewrite()
{

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    sfinfo.samplerate = d_h.sample_rate;
    sfinfo.channels = d_h.nchans;
    switch (d_h.format) {
    case FORMAT_WAV:
        switch (d_h.subformat) {
        case FORMAT_PCM_U8:
            sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_U8);
            break;
        case FORMAT_PCM_16:
            sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_16);
            break;
        case FORMAT_PCM_24:
            sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_24);
            break;
        case FORMAT_PCM_32:
            sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_PCM_32);
            break;
        case FORMAT_FLOAT:
            sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_FLOAT);
            break;
        case FORMAT_DOUBLE:
            sfinfo.format = (SF_FORMAT_WAV | SF_FORMAT_DOUBLE);
            break;
        }
        break;
    case FORMAT_FLAC:
        switch (d_h.subformat) {
        case FORMAT_PCM_S8:
            sfinfo.format = (SF_FORMAT_FLAC | SF_FORMAT_PCM_S8);
            break;
        case FORMAT_PCM_16:
            sfinfo.format = (SF_FORMAT_FLAC | SF_FORMAT_PCM_16);
            break;
        case FORMAT_PCM_24:
            sfinfo.format = (SF_FORMAT_FLAC | SF_FORMAT_PCM_24);
            break;
        }
        break;
    case FORMAT_OGG:
        switch (d_h.subformat) {
        case FORMAT_VORBIS:
            sfinfo.format = (SF_FORMAT_OGG | SF_FORMAT_VORBIS);
            break;
        case FORMAT_OPUS:
#ifdef HAVE_SF_FORMAT_OPUS
            sfinfo.format = (SF_FORMAT_OGG | SF_FORMAT_OPUS);
#else
            throw std::runtime_error("libsndfile < 1.0.29 does not support Opus.");
#endif
            break;
        }
        break;
    case FORMAT_RF64:
        switch (d_h.subformat) {
        case FORMAT_PCM_U8:
            sfinfo.format = (SF_FORMAT_RF64 | SF_FORMAT_PCM_U8);
            break;
        case FORMAT_PCM_16:
            sfinfo.format = (SF_FORMAT_RF64 | SF_FORMAT_PCM_16);
            break;
        case FORMAT_PCM_24:
            sfinfo.format = (SF_FORMAT_RF64 | SF_FORMAT_PCM_24);
            break;
        case FORMAT_PCM_32:
            sfinfo.format = (SF_FORMAT_RF64 | SF_FORMAT_PCM_32);
            break;
        case FORMAT_FLOAT:
            sfinfo.format = (SF_FORMAT_RF64 | SF_FORMAT_FLOAT);
            break;
        case FORMAT_DOUBLE:
            sfinfo.format = (SF_FORMAT_RF64 | SF_FORMAT_DOUBLE);
            break;
        }
        break;
    }
    errno = 0;
    if (!(d_new_fp = sf_open(d_filename.c_str(), SFM_WRITE, &sfinfo))) {
        if (errno) {
            d_logger->error("sf_open(2) failed: {:s}: {:s}", d_filename, strerror(errno));
        } else {
            d_logger->error(
                "sf_open(2) failed: {:s}: {:s}", d_filename, sf_strerror(NULL));
        }

        return false;
    }

    d_updated = true;
    return true;
}

void wavfile_sink_impl::close()
{
    gr::thread::scoped_lock guard(d_mutex);

    if (!d_fp) {
        return;
    }
    close_wav();
}

void wavfile_sink_impl::close_wav()
{
    sf_write_sync(d_fp);
    sf_close(d_fp);
    d_fp = nullptr;
}

wavfile_sink_impl::~wavfile_sink_impl() { stop(); }

bool wavfile_sink_impl::stop()
{
    if (d_new_fp) {
        sf_close(d_new_fp);
        d_new_fp = nullptr;
    }
    close();
    d_should_reopen = true;

    return true;
}

bool wavfile_sink_impl::start()
{
    gr::thread::scoped_lock guard(d_mutex);
    if (d_should_reopen) {
        d_should_reopen = false;
        if (!open_file_for_append()) {
            d_logger->error("Failed to reopen output file after reconfiguration.");
            return false;
        }
    }

    return true;
}

int wavfile_sink_impl::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    auto in = (const float**)&input_items[0];
    int n_in_chans = input_items.size();
    int nwritten;
    int errnum;

    gr::thread::scoped_lock guard(d_mutex); // hold mutex for duration of this block
    do_update();                            // update: d_fp is read
    if (!d_fp) {                            // drop output on the floor
        return noutput_items;
    }

    int nchans = d_h.nchans;
    for (nwritten = 0; nwritten < noutput_items; nwritten++) {
        for (int chan = 0; chan < nchans; chan++) {
            // Write zeros to channels which are in the WAV file
            // but don't have any inputs here
            if (chan < n_in_chans) {
                d_buffer[chan + (nwritten * nchans)] = in[chan][nwritten];
            } else {
                d_buffer[chan + (nwritten * nchans)] = 0;
            }
        }
    }

    sf_write_float(d_fp, &d_buffer[0], nchans * nwritten);

    errnum = sf_error(d_fp);
    if (errnum) {
        d_logger->error("sf_error: {:s}", sf_error_number(errnum));
        close();
        throw std::runtime_error("File I/O error.");
    }

    return nwritten;
}

void wavfile_sink_impl::set_bits_per_sample(int bits_per_sample)
{
    d_logger->warn("set_bits_per_sample() does nothing. Sample size is controlled by the "
                   "subformat.");
}

void wavfile_sink_impl::set_append(bool append)
{
    gr::thread::scoped_lock guard(d_mutex);
    d_append = append;
}

void wavfile_sink_impl::set_sample_rate(unsigned int sample_rate)
{
    gr::thread::scoped_lock guard(d_mutex);
    d_h.sample_rate = sample_rate;
}

int wavfile_sink_impl::bits_per_sample() { return d_h.bytes_per_sample * 8; }

unsigned int wavfile_sink_impl::sample_rate() { return d_h.sample_rate; }

void wavfile_sink_impl::do_update()
{
    if (!d_updated) {
        return;
    }

    if (d_fp) {
        close_wav();
    }

    d_fp = d_new_fp; // install new file pointer
    d_new_fp = nullptr;
    d_updated = false;
}

} /* namespace blocks */
} /* namespace gr */
