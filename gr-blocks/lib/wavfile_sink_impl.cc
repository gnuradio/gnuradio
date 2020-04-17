/* -*- c++ -*- */
/*
 * Copyright 2004,2006-2011,2013 Free Software Foundation, Inc.
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
#include <gnuradio/blocks/wavfile.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/thread/thread.h>
#include <fcntl.h>
#include <boost/math/special_functions/round.hpp>
#include <climits>
#include <cmath>
#include <cstring>
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

wavfile_sink::sptr wavfile_sink::make(const char* filename,
                                      int n_channels,
                                      unsigned int sample_rate,
                                      int bits_per_sample,
                                      bool append)
{
    return gnuradio::get_initial_sptr(new wavfile_sink_impl(
        filename, n_channels, sample_rate, bits_per_sample, append));
}

wavfile_sink_impl::wavfile_sink_impl(const char* filename,
                                     int n_channels,
                                     unsigned int sample_rate,
                                     int bits_per_sample,
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
    d_h.sample_rate = sample_rate;
    d_h.nchans = n_channels;

    set_bits_per_sample_unlocked(bits_per_sample);

    d_h.bytes_per_sample = d_bytes_per_sample_new;

    if (!open(filename)) {
        throw std::runtime_error("can't open WAV file");
    }
}

bool wavfile_sink_impl::open(const char* filename)
{
    gr::thread::scoped_lock guard(d_mutex);

    // we use the open system call to get access to the O_LARGEFILE flag.
    int flags = OUR_O_LARGEFILE | OUR_O_BINARY;

    if (!d_append) {
        // We are generating a new file.
        flags |= O_CREAT | O_WRONLY | O_TRUNC;
    } else {
        flags |= O_RDWR;
    }

    int fd;
    if ((fd = ::open(filename, flags, 0664)) < 0) {
        if (errno == ENOENT) {
            throw std::runtime_error("WAV append mode requires target file to exist");
        } else {
            GR_LOG_ERROR(d_logger,
                         boost::format("::open: %s: %s") % filename % strerror(errno));
        }
        return false;
    }

    if (d_new_fp) { // if we've already got a new one open, close it
        fclose(d_new_fp);
        d_new_fp = nullptr;
    }

    if (!(d_new_fp = fdopen(fd, d_append ? "r+b" : "wb"))) {
        GR_LOG_ERROR(d_logger,
                     boost::format("fdopen: %s: %s") % filename % strerror(errno));

        ::close(fd); // don't leak file descriptor if fdopen fails.
        return false;
    }

    if (d_append) {
        // We are appending to an existing file, be extra careful here.
        if (!check_append_compat_file(d_new_fp)) {
            fclose(d_new_fp);
            return false;
        }
    } else {
        d_h.first_sample_pos = 44;
        if (!wavheader_write(
                d_new_fp, d_h.sample_rate, d_h.nchans, d_bytes_per_sample_new)) {
            GR_LOG_ERROR(d_logger, "could not save WAV header");
            fclose(d_new_fp);
            return false;
        }
    }

    d_updated = true;

    return true;
}

bool wavfile_sink_impl::check_append_compat_file(FILE* fp)
{

    if (d_bytes_per_sample_new != d_h.bytes_per_sample) {
        GR_LOG_ERROR(d_logger,
                     "bytes_per_sample is not allowed to change in append mode");
        return false;
    }

    wav_header_info h_tmp{};
    std::swap(d_h, h_tmp);

    if (!wavheader_parse(fp, d_h)) {
        GR_LOG_ERROR(d_logger, "invalid or incompatible WAV file");
        return false;
    }

    if (d_h.sample_rate != h_tmp.sample_rate || d_h.nchans != h_tmp.nchans ||
        d_h.bytes_per_sample != h_tmp.bytes_per_sample) {
        GR_LOG_ERROR(d_logger,
                     "existing WAV file is incompatible with configured options");
        return false;
    }

    // TODO: use GR_FSEEK, GR_FTELL.
    if (fseek(d_new_fp, 0, SEEK_END) != 0) {
        return false; // This can only happen if the file disappears under our feet.
    }

    long file_size = ftell(fp);
    if (file_size - d_h.first_sample_pos != d_h.data_chunk_size) {
        // This is complicated to properly implement for too little benefit.
        GR_LOG_ERROR(d_logger,
                     "existing WAV file is incompatible (extra chunks at the end)");
        return false;
    }

    return true;
}


void wavfile_sink_impl::close()
{
    gr::thread::scoped_lock guard(d_mutex);

    if (!d_fp)
        return;

    close_wav();
}

void wavfile_sink_impl::close_wav()
{
    if (!wavheader_complete(d_fp, d_h.first_sample_pos)) {
        GR_LOG_ERROR(d_logger, boost::format("could not save WAV header"));
    }

    fclose(d_fp);
    d_fp = nullptr;
}

wavfile_sink_impl::~wavfile_sink_impl() { stop(); }

bool wavfile_sink_impl::stop()
{
    if (d_new_fp) {
        fclose(d_new_fp);
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

    short int sample_buf_s;

    int nwritten;

    gr::thread::scoped_lock guard(d_mutex); // hold mutex for duration of this block
    do_update();                            // update: d_fp is read
    if (!d_fp)                              // drop output on the floor
        return noutput_items;

    int nchans = d_h.nchans;
    int bytes_per_sample = d_h.bytes_per_sample;
    for (nwritten = 0; nwritten < noutput_items; nwritten++) {
        for (int chan = 0; chan < nchans; chan++) {
            // Write zeros to channels which are in the WAV file
            // but don't have any inputs here
            if (chan < n_in_chans) {
                sample_buf_s = convert_to_short(in[chan][nwritten]);
            } else {
                sample_buf_s = 0;
            }

            wav_write_sample(d_fp, sample_buf_s, bytes_per_sample);

            if (feof(d_fp) || ferror(d_fp)) {
                GR_LOG_ERROR(d_logger,
                             boost::format("file i/o error %s") % strerror(errno));
                close();
                exit(-1);
            }
        }
    }

    return nwritten;
}

short int wavfile_sink_impl::convert_to_short(float sample)
{
    sample += d_normalize_shift;
    sample *= d_normalize_fac;

    // In C++17, use std::clamp.
    sample = std::max(std::min(sample, d_max_sample_val), d_min_sample_val);

    return (short int)boost::math::iround(sample);
}

void wavfile_sink_impl::set_bits_per_sample(int bits_per_sample)
{
    gr::thread::scoped_lock guard(d_mutex);
    set_bits_per_sample_unlocked(bits_per_sample);
}

void wavfile_sink_impl::set_bits_per_sample_unlocked(int bits_per_sample)
{
    switch (bits_per_sample) {
    case 8:
        d_max_sample_val = float(UINT8_MAX);
        d_min_sample_val = float(0);
        d_normalize_fac = d_max_sample_val / 2;
        d_normalize_shift = 1;
        break;
    case 16:
        d_max_sample_val = float(INT16_MAX);
        d_min_sample_val = float(INT16_MIN);
        d_normalize_fac = d_max_sample_val;
        d_normalize_shift = 0;
        break;
    default:
        throw std::runtime_error("Invalid bits per sample (only 8 and 16 are supported)");
    }
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
