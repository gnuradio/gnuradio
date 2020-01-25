/* -*- c++ -*- */
/*
 * Copyright 2004-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_OSS_SINK_H
#define INCLUDED_AUDIO_OSS_SINK_H

#include <gnuradio/audio/sink.h>
#include <string>

namespace gr {
namespace audio {

/*!
 * \brief audio sink using OSS
 * \ingroup audio_blk
 *
 * input signature is one or two streams of floats.
 * Input samples must be in the range [-1,1].
 */
class oss_sink : public sink
{
    int d_sampling_rate;
    std::string d_device_name;
    int d_fd;
    short* d_buffer;
    int d_chunk_size;

public:
    oss_sink(int sampling_rate,
             const std::string device_name = "",
             bool ok_to_block = true);
    ~oss_sink();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_OSS_SINK_H */
