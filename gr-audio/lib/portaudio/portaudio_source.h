/* -*- c++ -*- */
/*
 * Copyright 2006-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_PORTAUDIO_SOURCE_H
#define INCLUDED_AUDIO_PORTAUDIO_SOURCE_H

#include <gnuradio/audio/source.h>
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/logger.h>
#include <gnuradio/thread/thread.h>
#include <portaudio.h>
#include <stdexcept>
#include <string>

namespace gr {
namespace audio {

PaStreamCallback portaudio_source_callback;

/*!
 * \brief Audio source using PORTAUDIO
 * \ingroup audio_blk
 *
 * Input samples must be in the range [-1,1].
 */
class portaudio_source : public source
{
    friend PaStreamCallback portaudio_source_callback;

    unsigned int d_sampling_rate;
    std::string d_device_name;
    bool d_ok_to_block;
    bool d_verbose;

    unsigned int d_portaudio_buffer_size_frames; // number of frames in a portaudio buffer

    PaStream* d_stream;
    PaStreamParameters d_input_parameters;

    gr::buffer_sptr d_writer; // buffer used between work and callback
    gr::buffer_reader_sptr d_reader;

    gr::thread::mutex d_ringbuffer_mutex;
    gr::thread::condition_variable d_ringbuffer_cond;
    bool d_ringbuffer_ready;

    // random stats
    int d_noverruns; // count of overruns

    void output_error_msg(const char* msg, int err);
    void bail(const char* msg, int err);
    void create_ringbuffer();

public:
    portaudio_source(int sampling_rate, const std::string device_name, bool ok_to_block);

    ~portaudio_source() override;

    bool check_topology(int ninputs, int noutputs) override;

    int work(int ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_PORTAUDIO_SOURCE_H */
