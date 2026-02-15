/* -*- c++ -*- */
/*
 * Copyright 2004-2026 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pulse_sink.h"
#include <gnuradio/prefs.h>
#include <unistd.h>
#include <algorithm>
#include <stdexcept>

// ────────────────────────────────────────────────
// Helpers for formatting PulseAudio state enums
// ────────────────────────────────────────────────
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

sink::sptr pulse_sink_fcn(int sampling_rate,
                          const std::string& device_name,
                          bool ok_to_block,
                          const std::map<std::string, std::string>& properties)
{
    return pulse_sink::make(sampling_rate, device_name, ok_to_block, properties);
}

pulse_sink::sptr pulse_sink::make(int sampling_rate,
                                  const std::string& device_name,
                                  bool ok_to_block,
                                  const std::map<std::string, std::string>& properties)
{
    return gnuradio::make_block_sptr<pulse_sink>(
        sampling_rate, device_name, ok_to_block, properties);
}

static std::string default_device_name()
{
    std::string dev =
        prefs::singleton()->get_string("audio_pulse", "default_output_device", "");
    return dev;
}

static double default_buffer_time()
{
    double t = prefs::singleton()->get_double("audio_pulse", "buffer_time", 0.020);
    return std::max(0.005, t);
}

pulse_sink::pulse_sink(int sampling_rate,
                       const std::string& device_name,
                       bool ok_to_block,
                       const std::map<std::string, std::string>& properties)
    : sync_block("pulse_sink",
                 io_signature::make(1, -1, sizeof(float)),
                 io_signature::make(0, 0, 0)),
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

    double buf_time = default_buffer_time();
    d_logger->debug("target buffer time: {} seconds", buf_time);

    // Initialize buffer attributes to "default/ignore" state
    d_ba.maxlength = (uint32_t)-1;
    d_ba.tlength = (uint32_t)-1;
    d_ba.prebuf = (uint32_t)-1;
    d_ba.minreq = (uint32_t)-1;
    d_ba.fragsize = (uint32_t)-1;

    d_logger->debug("buffer attributes → tlength = {} bytes", d_ba.tlength);

    init_pa();

    d_logger->debug("construction completed");
}

pulse_sink::~pulse_sink()
{
    d_logger->debug("destructor called");
    cleanup();
}

void pulse_sink::cleanup()
{
    d_logger->debug("cleanup started");

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
        pa_threaded_mainloop_stop(d_mainloop);
        pa_threaded_mainloop_free(d_mainloop);
        d_mainloop = nullptr;
        d_logger->debug("mainloop stopped and freed");
    }

    d_logger->debug("cleanup completed");
}

void pulse_sink::init_pa()
{
    d_logger->debug("initializing connection");

    d_mainloop = pa_threaded_mainloop_new();
    if (!d_mainloop) {
        d_logger->error("pa_threaded_mainloop_new() failed");
        throw std::runtime_error("mainloop_new failed");
    }

    pa_mainloop_api* api = pa_threaded_mainloop_get_api(d_mainloop);
    if (!api) {
        d_logger->error("pa_threaded_mainloop_get_api() returned null");
        throw std::runtime_error("mainloop api null");
    }

    d_context = pa_context_new(api, "GNU Radio");
    if (!d_context) {
        d_logger->error("pa_context_new() failed");
        throw std::runtime_error("context_new failed");
    }

    pa_context_set_state_callback(d_context, context_state_cb, this);

    if (pa_threaded_mainloop_start(d_mainloop) < 0) {
        d_logger->error("pa_threaded_mainloop_start() failed");
        throw std::runtime_error("mainloop start failed");
    }

    pa_threaded_mainloop_lock(d_mainloop);

    if (pa_context_connect(d_context, nullptr, PA_CONTEXT_NOFAIL, nullptr) < 0) {
        int err = pa_context_errno(d_context);
        d_logger->error("pa_context_connect() failed: {}", pa_strerror(err));
        pa_threaded_mainloop_unlock(d_mainloop);
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
            pa_threaded_mainloop_unlock(d_mainloop);
            throw std::runtime_error("context failed");
        }
        pa_threaded_mainloop_wait(d_mainloop);
    }

    pa_threaded_mainloop_unlock(d_mainloop);
    d_logger->debug("context is READY");
}

bool pulse_sink::check_topology(int ninputs, int noutputs)
{
    d_logger->debug("check_topology(ninputs={}, noutputs={})", ninputs, noutputs);

    if (ninputs < 1) {
        d_logger->error("check_topology: no input streams connected");
        return false;
    }

    d_nchan = ninputs;
    d_ss.channels = d_nchan;

    double buf_time = default_buffer_time();
    d_ba.tlength =
        static_cast<uint32_t>(d_sampling_rate * buf_time * sizeof(float) * d_nchan);
    d_ba.minreq =
        static_cast<uint32_t>(d_sampling_rate * 0.005 * sizeof(float) * d_nchan);
    d_ba.prebuf = d_ba.minreq;

    d_logger->debug("topology: {} channels, tlength={} B, minreq={} B",
                    d_nchan,
                    d_ba.tlength,
                    d_ba.minreq);


    pa_proplist* pl = pa_proplist_new();
    pa_proplist_sets(pl, PA_PROP_MEDIA_NAME, "GNU Radio - Playback");
    pa_proplist_sets(pl, PA_PROP_APPLICATION_NAME, "GNU Radio");
    pa_proplist_sets(
        pl, PA_PROP_APPLICATION_PROCESS_ID, std::to_string(getpid()).c_str());

    for (const auto& [k, v] : d_properties) {
        pa_proplist_sets(pl, k.c_str(), v.c_str());
    }

    const char* final_name = pa_proplist_gets(pl, PA_PROP_MEDIA_NAME);

    pa_threaded_mainloop_lock(d_mainloop);

    d_stream = pa_stream_new_with_proplist(d_context, final_name, &d_ss, nullptr, pl);
    pa_proplist_free(pl);

    if (!d_stream) {
        int err = pa_context_errno(d_context);
        d_logger->error("pa_stream_new_with_proplist failed: {}", pa_strerror(err));
        pa_threaded_mainloop_unlock(d_mainloop);
        return false;
    }

    pa_stream_set_state_callback(d_stream, stream_state_cb, this);
    pa_stream_set_write_callback(d_stream, stream_write_cb, this);
    pa_stream_set_underflow_callback(d_stream, stream_underflow_cb, this);

    pa_stream_flags_t flags = static_cast<pa_stream_flags_t>(
        PA_STREAM_ADJUST_LATENCY | PA_STREAM_AUTO_TIMING_UPDATE |
        PA_STREAM_INTERPOLATE_TIMING);

    if (!d_ok_to_block) {
        flags =
            static_cast<pa_stream_flags_t>(flags | PA_STREAM_DONT_INHIBIT_AUTO_SUSPEND);
    }

    const char* dev_str = d_device_name.empty() ? nullptr : d_device_name.c_str();
    if (pa_stream_connect_playback(d_stream, dev_str, &d_ba, flags, nullptr, nullptr) <
        0) {
        int err = pa_context_errno(d_context);
        d_logger->error("pa_stream_connect_playback failed: {}", pa_strerror(err));
        pa_threaded_mainloop_unlock(d_mainloop);
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
            pa_threaded_mainloop_unlock(d_mainloop);
            return false;
        }
        pa_threaded_mainloop_wait(d_mainloop);
    }

    pa_operation* op = pa_stream_flush(d_stream, nullptr, nullptr);
    if (op)
        pa_operation_unref(op);

    pa_threaded_mainloop_unlock(d_mainloop);

    d_logger->debug("check_topology succeeded - stream is READY");
    return true;
}

int pulse_sink::work(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items)
{
    if (!d_stream) {
        d_logger->warn("work() called but no stream");
        return 0;
    }

    const float** in = reinterpret_cast<const float**>(input_items.data());
    int processed = 0;

    pa_threaded_mainloop_lock(d_mainloop);

    while (processed < noutput_items) {
        // 1. Check if the hardware is ready for more data
        size_t writable_bytes = pa_stream_writable_size(d_stream);

        if (writable_bytes == 0) {
            if (d_ok_to_block) {
                // Wait for the write_cb to signal that space is available
                while (pa_stream_writable_size(d_stream) == 0 &&
                       pa_stream_get_state(d_stream) == PA_STREAM_READY) {
                    pa_threaded_mainloop_wait(d_mainloop);
                }
            } else {
                // Non-blocking: yield to scheduler if buffer is full
                break;
            }
        }

        // 2. Safety Check: Did the stream die while we waited?
        if (pa_stream_get_state(d_stream) != PA_STREAM_READY) {
            d_logger->warn("Stream state changed to {} during write",
                           pa_stream_state_str(pa_stream_get_state(d_stream)));
            break;
        }

        // 3. Determine how many samples we can actually write right now
        writable_bytes = pa_stream_writable_size(d_stream);
        size_t samples_hardware_can_take = writable_bytes / (sizeof(float) * d_nchan);
        size_t samples_remaining = noutput_items - processed;
        size_t samples_to_write = std::min(samples_remaining, samples_hardware_can_take);

        if (samples_to_write == 0)
            break;

        // 4. Directly request a buffer from PulseAudio (Zero-Copy)
        void* pa_buf = nullptr;
        size_t nbytes = samples_to_write * d_nchan * sizeof(float);
        if (pa_stream_begin_write(d_stream, &pa_buf, &nbytes) < 0) {
            d_logger->error("pa_stream_begin_write failed");
            break;
        }

        // 5. Re-calculate based on what PulseAudio actually gave us
        size_t actual_samples = nbytes / (sizeof(float) * d_nchan);
        float* dst = static_cast<float*>(pa_buf);

        for (size_t i = 0; i < actual_samples; ++i) {
            for (unsigned int chan = 0; chan < d_nchan; ++chan) {
                dst[i * d_nchan + chan] = in[chan][processed + i];
            }
        }

        // 6. Commit the data to the sound card
        if (pa_stream_write(d_stream, pa_buf, nbytes, nullptr, 0, PA_SEEK_RELATIVE) < 0) {
            d_logger->error("pa_stream_write failed");
            break;
        }

        processed += actual_samples;
    }

    pa_threaded_mainloop_unlock(d_mainloop);

    return processed;
}

// ────────────────────────────────────────────────
// Callbacks
// ────────────────────────────────────────────────

void pulse_sink::context_state_cb(pa_context* c, void* u)
{
    auto* self = static_cast<pulse_sink*>(u);
    pa_context_state_t state = pa_context_get_state(c);
    self->d_logger->debug("context state → {}", pa_context_state_str(state));
    pa_threaded_mainloop_signal(self->d_mainloop, 0);
}

void pulse_sink::stream_state_cb(pa_stream* s, void* u)
{
    auto* self = static_cast<pulse_sink*>(u);
    pa_stream_state_t state = pa_stream_get_state(s);
    self->d_logger->debug("stream state → {}", pa_stream_state_str(state));
    pa_threaded_mainloop_signal(self->d_mainloop, 0);
}

void pulse_sink::stream_write_cb(pa_stream* /*s*/, size_t length, void* u)
{
    auto* self = static_cast<pulse_sink*>(u);
    pa_threaded_mainloop_signal(self->d_mainloop, 0);
}

void pulse_sink::stream_underflow_cb(pa_stream* /*s*/, void* u)
{
    auto* self = static_cast<pulse_sink*>(u);
    self->d_logger->warn("underflow detected");
}

} // namespace audio
} // namespace gr
