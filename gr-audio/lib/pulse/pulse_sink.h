/* -*- c++ -*- */
/*
 * Copyright 2004-2026 Free Software Foundation, Inc.
 * Copyright 2026 Contributors (low-latency, robust fixes, dynamic properties)
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_PULSE_SINK_H
#define INCLUDED_AUDIO_PULSE_SINK_H

#include <gnuradio/audio/sink.h>
#include <gnuradio/logger.h>
#include <pulse/thread-mainloop.h>
#include <pulse/context.h>
#include <pulse/stream.h>
#include <pulse/sample.h>
#include <pulse/error.h>
#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace gr {
namespace audio {

/*!
 * \brief PulseAudio sink using full threaded API (low-latency, robust)
 * \ingroup audio_blk
 */
class pulse_sink : public sink
{
public:
    typedef std::shared_ptr<pulse_sink> sptr;

    static sptr make(int sampling_rate,
                     const std::string& device_name = "",
                     bool ok_to_block = true,
                     const std::map<std::string, std::string>& properties = {});

    pulse_sink(int sampling_rate,
               const std::string& device_name,
               bool ok_to_block,
               const std::map<std::string, std::string>& properties);

    ~pulse_sink() override;

    bool check_topology(int ninputs, int noutputs) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

private:
    void init_pa();
    void cleanup();

    static void context_state_cb(pa_context* c, void* userdata);
    static void stream_state_cb(pa_stream* s, void* userdata);
    static void stream_write_cb(pa_stream* s, size_t length, void* userdata);
    static void stream_underflow_cb(pa_stream* s, void* userdata);

    void output_error(const std::string& msg, int error);

    int d_sampling_rate;
    std::string d_device_name;
    bool d_ok_to_block;
    std::map<std::string, std::string> d_properties;

    pa_threaded_mainloop* d_mainloop;
    pa_context* d_context;
    pa_stream* d_stream;

    pa_sample_spec d_ss;
    pa_buffer_attr d_ba;

    size_t d_chunk_size;
    unsigned int d_nchan;

    std::vector<float> d_buffer;  // for interleaving channels
    bool d_writable;
    std::mutex d_mutex;
    std::condition_variable d_cond;
};

} // namespace audio
} // namespace gr

#endif /* INCLUDED_AUDIO_PULSE_SINK_H */
