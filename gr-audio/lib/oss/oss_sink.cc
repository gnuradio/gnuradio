/* -*- c++ -*- */
/*
 * Copyright 2004-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "../audio_registry.h"
#include "oss_sink.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>

namespace gr {
namespace audio {

sink::sptr
oss_sink_fcn(int sampling_rate, const std::string& device_name, bool ok_to_block)
{
    return sink::sptr(new oss_sink(sampling_rate, device_name, ok_to_block));
}

static std::string default_device_name()
{
    return prefs::singleton()->get_string(
        "audio_oss", "default_output_device", "/dev/dsp");
}

oss_sink::oss_sink(int sampling_rate, const std::string device_name, bool ok_to_block)
    : sync_block("audio_oss_sink",
                 io_signature::make(1, 2, sizeof(float)),
                 io_signature::make(0, 0, 0)),
      d_sampling_rate(sampling_rate),
      d_device_name(device_name.empty() ? default_device_name() : device_name),
      d_fd(-1),
      d_buffer(0),
      d_chunk_size(0)
{
    if ((d_fd = open(d_device_name.c_str(), O_WRONLY)) < 0) {
        GR_LOG_ERROR(d_logger,
                     boost::format("opening device %s: %s") % d_device_name %
                         strerror(errno));
        throw std::runtime_error("audio_oss_sink");
    }

    double CHUNK_TIME =
        std::max(0.001, prefs::singleton()->get_double("audio_oss", "latency", 0.005));

    d_chunk_size = (int)(d_sampling_rate * CHUNK_TIME);
    set_output_multiple(d_chunk_size);

    d_buffer = new short[d_chunk_size * 2];

    int format = AFMT_S16_NE;
    int orig_format = format;
    if (ioctl(d_fd, SNDCTL_DSP_SETFMT, &format) < 0) {
        GR_LOG_ERROR(d_logger,
                     boost::format("%s ioctl failed: %s") % d_device_name %
                         strerror(errno));
        throw std::runtime_error("audio_oss_sink");
    }

    if (format != orig_format) {
        GR_LOG_ERROR(
            d_logger,
            boost::format("%s unable to support format %d. card requested %d instead.") %
                orig_format % format);
    }

    // set to stereo no matter what.  Some hardware only does stereo
    int channels = 2;
    if (ioctl(d_fd, SNDCTL_DSP_CHANNELS, &channels) < 0 || channels != 2) {
        GR_LOG_ERROR(d_logger,
                     boost::format("could not set STEREO mode: %s") % strerror(errno));
        throw std::runtime_error("audio_oss_sink");
    }

    // set sampling freq
    int sf = sampling_rate;
    if (ioctl(d_fd, SNDCTL_DSP_SPEED, &sf) < 0) {
        GR_LOG_ERROR(d_logger,
                     boost::format("%s: invalid sampling_rate %d") % d_device_name %
                         sampling_rate);
        sampling_rate = 8000;
        if (ioctl(d_fd, SNDCTL_DSP_SPEED, &sf) < 0) {
            GR_LOG_ERROR(d_logger, "failed to set sampling_rate to 8000");
            throw std::runtime_error("audio_oss_sink");
        }
    }
}

oss_sink::~oss_sink()
{
    close(d_fd);
    delete[] d_buffer;
}

int oss_sink::work(int noutput_items,
                   gr_vector_const_void_star& input_items,
                   gr_vector_void_star& output_items)
{
    const float *f0, *f1;

    switch (input_items.size()) {
    case 1: // mono input
        f0 = (const float*)input_items[0];

        for (int i = 0; i < noutput_items; i += d_chunk_size) {
            for (int j = 0; j < d_chunk_size; j++) {
                d_buffer[2 * j + 0] = (short)(f0[j] * 32767);
                d_buffer[2 * j + 1] = (short)(f0[j] * 32767);
            }
            f0 += d_chunk_size;
            if (write(d_fd, d_buffer, 2 * d_chunk_size * sizeof(short)) < 0)
                GR_LOG_ERROR(d_logger, boost::format("write %s") % strerror(errno));
        }
        break;

    case 2: // stereo input
        f0 = (const float*)input_items[0];
        f1 = (const float*)input_items[1];

        for (int i = 0; i < noutput_items; i += d_chunk_size) {
            for (int j = 0; j < d_chunk_size; j++) {
                d_buffer[2 * j + 0] = (short)(f0[j] * 32767);
                d_buffer[2 * j + 1] = (short)(f1[j] * 32767);
            }
            f0 += d_chunk_size;
            f1 += d_chunk_size;
            if (write(d_fd, d_buffer, 2 * d_chunk_size * sizeof(short)) < 0)
                GR_LOG_ERROR(d_logger, boost::format("write %s") % strerror(errno));
        }
        break;
    }

    return noutput_items;
}

} /* namespace audio */
} /* namespace gr */
