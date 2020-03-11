/* -*- c++ -*- */
/*
 * Copyright 2004-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_OSS_SOURCE_H
#define INCLUDED_AUDIO_OSS_SOURCE_H

#include <gnuradio/audio/source.h>
#include <string>

namespace gr {
namespace audio {

/*!
 * \brief audio source using OSS
 * \ingroup audio_blk
 *
 * Output signature is one or two streams of floats.
 * Output samples will be in the range [-1,1].
 */
class oss_source : public source
{
    int d_sampling_rate;
    std::string d_device_name;
    int d_fd;
    short* d_buffer;
    int d_chunk_size;

public:
    oss_source(int sampling_rate,
               const std::string device_name = "",
               bool ok_to_block = true);

    ~oss_source();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_OSS_SOURCE_H */
