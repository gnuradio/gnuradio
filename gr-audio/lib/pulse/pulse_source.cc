/* -*- c++ -*- */
/*
 * Copyright 2004-2026 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pulse_source.h"
#include <gnuradio/prefs.h>
#include <unistd.h>
#include <algorithm>
#include <stdexcept>

static const char* pa_context_state_str(pa_context_state_t s)
{
    switch (s) {
    case PA_CONTEXT_UNCONNECTED:
        return "UNCONNECTED";
    case PA_CONTEXT_CONNECTING:
        return "CONNECTING";
    case PA_CONTEXT_AUTHORIZING:
        return "AUTHORIZING";
    case PA_CONTEXT_SETTING_NAME:
        return "SETTING_NAME";
    case PA_CONTEXT_FAILED:
        return "FAILED";
    case PA_CONTEXT_TERMINATED:
        return "TERMINATED";
    case PA_CONTEXT_READY:
        return "READY";
    default:
        return "???";
    }
}

static const char* pa_stream_state_str(pa_stream_state_t s)
{
    switch (s) {
    case PA_STREAM_UNCONNECTED:
        return "UNCONNECTED";
    case PA_STREAM_CREATING:
        return "CREATING";
    case PA_STREAM_READY:
        return "READY";
    case PA_STREAM_FAILED:
        return "FAILED";
    case PA_STREAM_TERMINATED:
        return "TERMINATED";
    default:
        return "???";
    }
}

namespace gr {
namespace audio {

source::sptr pulse_source_fcn(int sampling_rate,
                              const std::string& device_name,
                              bool ok_to_block,
                              const std::map<std::string, std::string>& properties)
{
    return pulse_source::make(sampling_rate, device_name, ok_to_block, properties);
}

pulse_source::sptr
pulse_source::make(int sampling_rate,
                   const std::string& device_name,
                   bool ok_to_block,
                   const std::map<std::string, std::string>& properties)
{
    return gnuradio::make_block_sptr<pulse_source>(
        sampling_rate, device_name, ok_to_block, properties);
}

static std::string default_device_name()
{
    std::string dev =
        prefs::singleton()->get_string("audio_pulse", "default_input_device", "");
    return dev;
}

static double default_buffer_time()
{
    double t = prefs::singleton()->get_double("audio_pulse", "buffer_time", 0.020);
    return std::max(0.005, t);
}

pulse_source::pulse_source(int sampling_rate,
                           const std::string& device_name,
                           bool ok_to_block,
                           const std::map<std::string, std::string>& properties)
    : sync_block("pulse_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, -1, sizeof(float))),
      d_sampling_rate(sampling_rate),
      d_device_name(device_name.empty() ? default_device_name() : device_name),
      d_ok_to_block(ok_to_block),
      d_properties(properties),
      d_mainloop(nullptr),
      d_context(nullptr),
      d_stream(nullptr),
      d_nchan(1)
{
    d_logger->debug("created - rate={}, device='{}', ok_to_block={}",
                    d_sampling_rate,
                    d_device_name,
                    d_ok_to_block);

    d_ss.format = PA_SAMPLE_FLOAT32NE;
    d_ss.rate = d_sampling_rate;

    d_ba.maxlength = (uint32_t)-1;
    d_ba.tlength = (uint32_t)-1;
    d_ba.prebuf = (uint32_t)-1;
    d_ba.minreq = (uint32_t)-1;
    d_ba.fragsize = (uint32_t)-1;

    d_logger->debug("construction completed");
}

pulse_source::~pulse_source()
{
    d_logger->debug("destructor called");
    stop();
}

bool pulse_source::check_topology(int ninputs, int noutputs)
{
    d_logger->debug("check_topology(ninputs={}, noutputs={})", ninputs, noutputs);

    if (noutputs < 1) {
        d_logger->error("check_topology: no output streams connected");
        return false;
    }

    d_nchan = noutputs;
    d_ss.channels = d_nchan;

    double buf_time = default_buffer_time();
    d_ba.tlength =
        static_cast<uint32_t>(d_sampling_rate * buf_time * sizeof(float) * d_nchan);
    d_ba.fragsize =
        static_cast<uint32_t>(d_sampling_rate * 0.005 * sizeof(float) * d_nchan);
    d_ba.prebuf = d_ba.fragsize;

    d_logger->debug("topology: {} channels, tlength={} B, fragsize={} B",
                    d_nchan,
                    d_ba.tlength,
                    d_ba.fragsize);

    return true;
}

bool pulse_source::start()
{
    d_logger->debug("start() called");

    try {
        init_pa();
    } catch (const std::exception& e) {
        d_logger->error("init_pa() failed: {}", e.what());
        return false;
    }

    return setup_stream();
}

bool pulse_source::stop()
{
    d_logger->debug("stop() called");

    if (d_stream) {
        pa_stream_disconnect(d_stream);
        pa_stream_unref(d_stream);
        d_stream = nullptr;
        d_logger->debug("stream disconnected and unreferenced");
    }

    if (d_context) {
        pa_context_disconnect(d_context);
        pa_context_unref(d_context);
        d_context = nullptr;
        d_logger->debug("context disconnected and unreferenced");
    }

    if (d_mainloop) {
        pa_mainloop_free(d_mainloop);
        d_mainloop = nullptr;
        d_logger->debug("mainloop freed");
    }

    d_residual_samples.clear();
    d_residual_offset = 0;

    d_logger->debug("stop() completed");
    return true;
}

void pulse_source::init_pa()
{
    d_logger->debug("initializing connection");

    d_mainloop = pa_mainloop_new();
    if (!d_mainloop) {
        d_logger->error("pa_mainloop_new() failed");
        throw std::runtime_error("mainloop_new failed");
    }

    pa_mainloop_api* api = pa_mainloop_get_api(d_mainloop);
    if (!api) {
        d_logger->error("pa_mainloop_get_api() returned null");
        throw std::runtime_error("mainloop api null");
    }

    d_context = pa_context_new(api, "GNU Radio");
    if (!d_context) {
        d_logger->error("pa_context_new() failed");
        throw std::runtime_error("context_new failed");
    }

    pa_context_set_state_callback(d_context, context_state_cb, this);

    if (pa_context_connect(d_context, nullptr, PA_CONTEXT_NOFAIL, nullptr) < 0) {
        int err = pa_context_errno(d_context);
        d_logger->error("pa_context_connect() failed: {}", pa_strerror(err));
        throw std::runtime_error("context connect failed");
    }

    d_logger->debug("waiting for context to become ready");
    while (true) {
        pa_context_state_t state = pa_context_get_state(d_context);
        d_logger->debug("context state: {}", pa_context_state_str(state));

        if (state == PA_CONTEXT_READY) {
            break;
        }
        if (state == PA_CONTEXT_FAILED || state == PA_CONTEXT_TERMINATED) {
            int err = pa_context_errno(d_context);
            d_logger->error("context failed or terminated: {}", pa_strerror(err));
            throw std::runtime_error("context failed");
        }

        int retval;
        if (pa_mainloop_iterate(d_mainloop, 1, &retval) < 0) {
            d_logger->error("pa_mainloop_iterate() failed");
            throw std::runtime_error("mainloop iterate failed");
        }
    }

    d_logger->debug("context is READY");
}

bool pulse_source::setup_stream()
{
    d_logger->debug("setting up stream");

    pa_proplist* pl = pa_proplist_new();
    pa_proplist_sets(pl, PA_PROP_MEDIA_NAME, "GNU Radio - Capture");
    pa_proplist_sets(pl, PA_PROP_APPLICATION_NAME, "GNU Radio");
    pa_proplist_sets(
        pl, PA_PROP_APPLICATION_PROCESS_ID, std::to_string(getpid()).c_str());

    for (const auto& [k, v] : d_properties) {
        pa_proplist_sets(pl, k.c_str(), v.c_str());
    }

    const char* final_name = pa_proplist_gets(pl, PA_PROP_MEDIA_NAME);

    d_stream = pa_stream_new_with_proplist(d_context, final_name, &d_ss, nullptr, pl);
    pa_proplist_free(pl);

    if (!d_stream) {
        int err = pa_context_errno(d_context);
        d_logger->error("pa_stream_new_with_proplist failed: {}", pa_strerror(err));
        return false;
    }

    pa_stream_set_state_callback(d_stream, stream_state_cb, this);
    pa_stream_set_read_callback(d_stream, stream_read_cb, this);
    pa_stream_set_underflow_callback(d_stream, stream_underflow_cb, this);
    pa_stream_set_overflow_callback(d_stream, stream_overflow_cb, this);

    pa_stream_flags_t flags = static_cast<pa_stream_flags_t>(
        PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE |
        PA_STREAM_INTERPOLATE_TIMING);

    const char* dev_str = d_device_name.empty() ? nullptr : d_device_name.c_str();
    if (pa_stream_connect_record(d_stream, dev_str, &d_ba, flags) < 0) {
        int err = pa_context_errno(d_context);
        d_logger->error("pa_stream_connect_record failed: {}", pa_strerror(err));
        return false;
    }

    d_logger->debug("waiting for stream state to be ready");
    while (true) {
        pa_stream_state_t state = pa_stream_get_state(d_stream);
        d_logger->debug("stream state: {}", pa_stream_state_str(state));

        if (state == PA_STREAM_READY)
            break;

        if (state == PA_STREAM_FAILED || state == PA_STREAM_TERMINATED) {
            int err = pa_context_errno(d_context);
            d_logger->error("stream failed or terminated: {}", pa_strerror(err));
            return false;
        }

        int retval;
        if (pa_mainloop_iterate(d_mainloop, 1, &retval) < 0) {
            d_logger->error("pa_mainloop_iterate() failed");
            return false;
        }
    }

    d_logger->debug("setup_stream succeeded - stream is READY");
    return true;
}

int pulse_source::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    if (!d_stream)
        return 0;

    float** out = reinterpret_cast<float**>(output_items.data());
    int produced = 0;

    if (!d_residual_samples.empty()) {
        size_t samples_in_res = (d_residual_samples.size() - d_residual_offset) / d_nchan;
        size_t to_copy = std::min((size_t)(noutput_items - produced), samples_in_res);

        for (size_t i = 0; i < to_copy; ++i) {
            for (unsigned int chan = 0; chan < d_nchan; ++chan) {
                out[chan][produced + i] =
                    d_residual_samples[d_residual_offset + i * d_nchan + chan];
            }
        }

        produced += to_copy;
        d_residual_offset += to_copy * d_nchan;

        if (d_residual_offset >= d_residual_samples.size()) {
            d_residual_samples.clear();
            d_residual_offset = 0;
        }
    }

    while (produced < noutput_items) {
        pa_stream_state_t stream_state = pa_stream_get_state(d_stream);
        if (stream_state != PA_STREAM_READY) {
            d_logger->warn("Stream state is {}", pa_stream_state_str(stream_state));
            break;
        }

        if (pa_stream_readable_size(d_stream) == 0) {
            if (d_ok_to_block) {
                int retval;
                if (pa_mainloop_iterate(d_mainloop, 1, &retval) < 0) {
                    d_logger->error("pa_mainloop_iterate() failed in work");
                    break;
                }
                continue;
            } else {
                break;
            }
        }

        const void* data = nullptr;
        size_t nbytes = 0;
        if (pa_stream_peek(d_stream, &data, &nbytes) < 0) {
            d_logger->error("pa_stream_peek failed");
            break;
        }

        if (nbytes == 0) {
            pa_stream_drop(d_stream);
            continue;
        }

        size_t samples_available = nbytes / (sizeof(float) * d_nchan);
        size_t space_left = noutput_items - produced;
        size_t to_process = std::min(space_left, samples_available);

        if (data == nullptr) {
            for (size_t i = 0; i < to_process; ++i) {
                for (unsigned int chan = 0; chan < d_nchan; ++chan) {
                    out[chan][produced + i] = 0.0f;
                }
            }
        } else {
            const float* src = static_cast<const float*>(data);

            for (size_t i = 0; i < to_process; ++i) {
                for (unsigned int chan = 0; chan < d_nchan; ++chan) {
                    out[chan][produced + i] = src[i * d_nchan + chan];
                }
            }

            if (to_process < samples_available) {
                size_t remaining_samples = (samples_available - to_process) * d_nchan;
                size_t start_index = to_process * d_nchan;

                d_residual_samples.assign(src + start_index,
                                          src + start_index + remaining_samples);
                d_residual_offset = 0;
            }
        }

        pa_stream_drop(d_stream);
        produced += to_process;

        if (!d_residual_samples.empty())
            break;
    }

    int retval;
    while (pa_mainloop_iterate(d_mainloop, 0, &retval) > 0) {
    }

    return produced;
}

void pulse_source::context_state_cb(pa_context* c, void* u)
{
    auto* self = static_cast<pulse_source*>(u);
    pa_context_state_t state = pa_context_get_state(c);
    self->d_logger->debug("context state -> {}", pa_context_state_str(state));
}

void pulse_source::stream_state_cb(pa_stream* s, void* u)
{
    auto* self = static_cast<pulse_source*>(u);
    pa_stream_state_t state = pa_stream_get_state(s);
    self->d_logger->debug("stream state -> {}", pa_stream_state_str(state));
}

void pulse_source::stream_read_cb(pa_stream* /*s*/, size_t nbytes, void* u)
{
    auto* self = static_cast<pulse_source*>(u);
    self->d_logger->debug("read callback: {} bytes available", nbytes);
}

void pulse_source::stream_underflow_cb(pa_stream* /*s*/, void* u)
{
    auto* self = static_cast<pulse_source*>(u);
    self->d_logger->warn("underflow detected");
}

void pulse_source::stream_overflow_cb(pa_stream* /*s*/, void* u)
{
    auto* self = static_cast<pulse_source*>(u);
    self->d_logger->warn("overflow detected");
}

} // namespace audio
} // namespace gr
