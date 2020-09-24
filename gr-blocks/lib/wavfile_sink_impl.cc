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
    int bits_per_sample;

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
    }
    set_bits_per_sample_unlocked(bits_per_sample);
    d_h.bytes_per_sample = d_bytes_per_sample_new;

    set_max_noutput_items(s_items_size);
    d_buffer.resize(s_items_size * d_h.nchans);

    if (!open(filename)) {
        throw std::runtime_error("Can't open WAV file.");
    }
}

bool wavfile_sink_impl::open(const char* filename)
{
    SF_INFO sfinfo;

    gr::thread::scoped_lock guard(d_mutex);

    if (d_new_fp) { // if we've already got a new one open, close it
        sf_close(d_new_fp);
        d_new_fp = nullptr;
    }

    if (d_append) {
        // We are appending to an existing file, be extra careful here.
        sfinfo.format = 0;
        if (!(d_new_fp = sf_open(filename, SFM_RDWR, &sfinfo))) {
            GR_LOG_ERROR(d_logger,
                         boost::format("sf_open failed: %s: %s") % filename %
                             strerror(errno));
            return false;
        }
        if (d_h.sample_rate != sfinfo.samplerate || d_h.nchans != sfinfo.channels ||
            d_h.format != (sfinfo.format & SF_FORMAT_TYPEMASK) ||
            d_h.subformat != (sfinfo.format & SF_FORMAT_SUBMASK)) {
            GR_LOG_ERROR(d_logger,
                         "Existing WAV file is incompatible with configured options.");
            sf_close(d_new_fp);
            return false;
        }
        if (sf_seek(d_new_fp, 0, SEEK_END) == -1) {
            GR_LOG_ERROR(d_logger, "Seek error.");
            return false; // This can only happen if the file disappears under our feet.
        }
    } else {
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
        if (!(d_new_fp = sf_open(filename, SFM_WRITE, &sfinfo))) {
            GR_LOG_ERROR(d_logger,
                         boost::format("sf_open failed: %s: %s") % filename %
                             strerror(errno));
            return false;
        }
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

    return true;
}

int wavfile_sink_impl::work(int noutput_items,
                            gr_vector_const_void_star& input_items,
                            gr_vector_void_star& output_items)
{
    auto in = (float**)&input_items[0];
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
        GR_LOG_ERROR(d_logger, boost::format("sf_error: %s") % sf_error_number(errnum));
        close();
        throw std::runtime_error("File I/O error.");
    }

    return nwritten;
}

void wavfile_sink_impl::set_bits_per_sample(int bits_per_sample)
{
    gr::thread::scoped_lock guard(d_mutex);
    set_bits_per_sample_unlocked(bits_per_sample);
}

void wavfile_sink_impl::set_bits_per_sample_unlocked(int bits_per_sample)
{
    d_bytes_per_sample_new = bits_per_sample / 8;
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

int wavfile_sink_impl::bits_per_sample() { return d_bytes_per_sample_new; }

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

    d_h.bytes_per_sample = d_bytes_per_sample_new;
    // Avoid deadlock.
    set_bits_per_sample_unlocked(8 * d_bytes_per_sample_new);
    d_updated = false;
}

} /* namespace blocks */
} /* namespace gr */
