/* -*- c++ -*- */
/*
 * Copyright 2006-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_AUDIO_PORTAUDIO_SINK_H
#define INCLUDED_AUDIO_PORTAUDIO_SINK_H

#include <gnuradio/audio/sink.h>
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_reader.h>
#include <gnuradio/logger.h>
#include <gnuradio/thread/thread.h>
#include <portaudio.h>
#include <stdexcept>
#include <string>
//#include <gri_logger.h>

namespace gr {
namespace audio {

PaStreamCallback portaudio_sink_callback;

/*!
 * \brief Audio sink using PORTAUDIO
 * \ingroup audio_blk
 *
 * Input samples must be in the range [-1,1].
 */
class portaudio_sink : public sink
{
    friend PaStreamCallback portaudio_sink_callback;

    unsigned int d_sampling_rate;
    std::string d_device_name;
    bool d_ok_to_block;
    bool d_verbose;

    unsigned int d_portaudio_buffer_size_frames; // number of frames in a portaudio buffer

    PaStream* d_stream;
    PaStreamParameters d_output_parameters;

    gr::buffer_sptr d_writer; // buffer used between work and callback
    gr::buffer_reader_sptr d_reader;

    gr::thread::mutex d_ringbuffer_mutex;
    gr::thread::condition_variable d_ringbuffer_cond;
    bool d_ringbuffer_ready;


    void output_error_msg(const char* msg, int err);
    void bail(const char* msg, int err);
    void create_ringbuffer();

public:
    portaudio_sink(int sampling_rate, const std::string device_name, bool ok_to_block);

    ~portaudio_sink() override;

    bool check_topology(int ninputs, int noutputs) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_PORTAUDIO_SINK_H */
