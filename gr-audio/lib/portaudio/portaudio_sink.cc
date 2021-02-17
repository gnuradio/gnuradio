/* -*- c++ -*- */
/*
 * Copyright 2006-2011,2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef _MSC_VER
#include <io.h>
#endif

#include "../audio_registry.h"
#include "portaudio_impl.h"
#include "portaudio_sink.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <future>
#include <iostream>
#include <stdexcept>
#ifdef _MSC_VER
#include <io.h>
#endif

namespace gr {
namespace audio {

sink::sptr
portaudio_sink_fcn(int sampling_rate, const std::string& device_name, bool ok_to_block)
{
    return sink::sptr(new portaudio_sink(sampling_rate, device_name, ok_to_block));
}

//#define LOGGING 0  // define to 0 or 1

#define SAMPLE_FORMAT paFloat32

typedef float sample_t;

// Number of portaudio buffers in the ringbuffer
static const unsigned int N_BUFFERS = 4;

static std::string default_device_name()
{
    return prefs::singleton()->get_string("audio_portaudio", "default_output_device", "");
}

void portaudio_sink::create_ringbuffer(void)
{
    int bufsize_samples =
        d_portaudio_buffer_size_frames * d_output_parameters.channelCount;

    if (d_verbose)
        GR_LOG_INFO(d_debug_logger,
                    boost::format("ring buffer size  = %d frames") %
                        (N_BUFFERS * bufsize_samples / d_output_parameters.channelCount));

    // FYI, the buffer indices are in units of samples.
    d_writer = gr::make_buffer(N_BUFFERS * bufsize_samples, sizeof(sample_t));
    d_reader = gr::buffer_add_reader(d_writer, 0);
}


void portaudio_underrun_notification(gr::logger_ptr logger)
{
    auto r = ::write(2, "aU", 2);
    if (r == -1) {
        GR_LOG_ERROR(logger, "portaudio_source_callback write error to stderr.");
    }
}
/*
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 *
 * Our job is to write framesPerBuffer frames into outputBuffer.
 */
int portaudio_sink_callback(const void* inputBuffer,
                            void* outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void* arg)
{
    auto self = reinterpret_cast<portaudio_sink*>(arg);
    int nreqd_samples = framesPerBuffer * self->d_output_parameters.channelCount;

    int navail_samples = self->d_reader->items_available();

    if (nreqd_samples <= navail_samples) { // We've got enough data...
        {
            gr::thread::scoped_lock guard(self->d_ringbuffer_mutex);

            memcpy(outputBuffer,
                   self->d_reader->read_pointer(),
                   nreqd_samples * sizeof(sample_t));
            self->d_reader->update_read_pointer(nreqd_samples);

            self->d_ringbuffer_ready = true;
        }

        // Tell the sink thread there is new room in the ringbuffer.
        self->d_ringbuffer_cond.notify_one();
        return paContinue;
    }

    else { // underrun
        auto future_local = std::async(&portaudio_underrun_notification, self->d_logger);
        // FIXME we should transfer what we've got and pad the rest
        memset(outputBuffer, 0, nreqd_samples * sizeof(sample_t));

        self->d_ringbuffer_ready = true;
        self->d_ringbuffer_cond.notify_one(); // Tell the sink to get going!

        return paContinue;
    }
}

// ----------------------------------------------------------------

portaudio_sink::portaudio_sink(int sampling_rate,
                               const std::string device_name,
                               bool ok_to_block)
    : sync_block("audio_portaudio_sink",
                 io_signature::make(0, 0, 0),
                 io_signature::make(0, 0, 0)),
      d_sampling_rate(sampling_rate),
      d_device_name(device_name.empty() ? default_device_name() : device_name),
      d_ok_to_block(ok_to_block),
      d_verbose(prefs::singleton()->get_bool("audio_portaudio", "verbose", false)),
      d_portaudio_buffer_size_frames(0),
      d_stream(0),
      d_ringbuffer_mutex(),
      d_ringbuffer_cond(),
      d_ringbuffer_ready(false)
{
    memset(&d_output_parameters, 0, sizeof(d_output_parameters));

    PaError err;
    int i, numDevices;
    PaDeviceIndex device = 0;
    const PaDeviceInfo* deviceInfo = NULL;

    err = Pa_Initialize();
    if (err != paNoError) {
        bail("Initialize failed", err);
    }

    if (d_verbose)
        print_devices();

    numDevices = Pa_GetDeviceCount();
    if (numDevices < 0)
        bail("Pa Device count failed", 0);
    if (numDevices == 0)
        bail("no devices available", 0);

    if (d_device_name.empty()) {
        // FIXME Get smarter about picking something
        GR_LOG_INFO(d_debug_logger, "Using Default Devicee");
        device = Pa_GetDefaultOutputDevice();
        deviceInfo = Pa_GetDeviceInfo(device);
        GR_LOG_ERROR(d_logger,
                     boost::format("%s is the chosen device using %s as the host") %
                         deviceInfo->name % Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
    } else {
        bool found = false;
        GR_LOG_INFO(d_debug_logger, "Test Devices");
        for (i = 0; i < numDevices; i++) {
            deviceInfo = Pa_GetDeviceInfo(i);
            GR_LOG_INFO(d_debug_logger,
                        boost::format("Testing device name: %s...") % deviceInfo->name);
            if (deviceInfo->maxOutputChannels <= 0) {
                continue;
            }

            if (strstr(deviceInfo->name, d_device_name.c_str())) {
                GR_LOG_INFO(d_debug_logger, "  Chosen!");
                device = i;
                GR_LOG_INFO(d_debug_logger,
                            boost::format("%s using %s as the host") %
                                d_device_name.c_str() %
                                Pa_GetHostApiInfo(deviceInfo->hostApi)->name);
                found = true;
                deviceInfo = Pa_GetDeviceInfo(device);
                i = numDevices; // force loop exit
            }
        }

        if (!found) {
            bail("Failed to find specified device name", 0);
            exit(1);
        }
    }

    d_output_parameters.device = device;
    d_output_parameters.channelCount = deviceInfo->maxOutputChannels;
    d_output_parameters.sampleFormat = SAMPLE_FORMAT;
    d_output_parameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
    d_output_parameters.hostApiSpecificStreamInfo = NULL;

    // We fill in the real channelCount in check_topology when we know
    // how many inputs are connected to us.

    // Now that we know the maximum number of channels (allegedly)
    // supported by the h/w, we can compute a reasonable input
    // signature.  The portaudio specs say that they'll accept any
    // number of channels from 1 to max.
    set_input_signature(
        io_signature::make(1, deviceInfo->maxOutputChannels, sizeof(sample_t)));
}

bool portaudio_sink::check_topology(int ninputs, int noutputs)
{
    PaError err;

    if (Pa_IsStreamActive(d_stream)) {
        Pa_CloseStream(d_stream);
        d_stream = 0;
        d_reader.reset(); // std::shared_ptr for d_reader = 0
        d_writer.reset(); // std::shared_ptr for d_write = 0
    }

    d_output_parameters.channelCount = ninputs; // # of channels we're really using

#if 1
    d_portaudio_buffer_size_frames =
        (int)(0.0213333333 * d_sampling_rate + 0.5); // Force 1024 frame buffers at 48000
    GR_LOG_ERROR(d_logger,
                 boost::format("Latency = %8.5f, requested sampling_rate = %g") %
                     0.0213333333 % (double)d_sampling_rate);
#endif
    err = Pa_OpenStream(&d_stream,
                        NULL, // No input
                        &d_output_parameters,
                        d_sampling_rate,
                        d_portaudio_buffer_size_frames,
                        paClipOff,
                        &portaudio_sink_callback,
                        (void*)this);

    if (err != paNoError) {
        output_error_msg("OpenStream failed", err);
        return false;
    }

#if 0
        const PaStreamInfo *psi = Pa_GetStreamInfo(d_stream);

        d_portaudio_buffer_size_frames = (int)(d_output_parameters.suggestedLatency  * psi->sampleRate);
        GR_LOG_ERROR(
            d_logger,
            boost::format("Latency = %7.4f, psi->sampleRate = %g") %
            d_input_parameters.suggestedLatency
            % psi->sampleRate
        );
#endif
    GR_LOG_ERROR(d_logger,
                 boost::format("d_portaudio_buffer_size_frames = %d") %
                     d_portaudio_buffer_size_frames);

    assert(d_portaudio_buffer_size_frames != 0);

    create_ringbuffer();

    err = Pa_StartStream(d_stream);
    if (err != paNoError) {
        output_error_msg("StartStream failed", err);
        return false;
    }

    return true;
}

portaudio_sink::~portaudio_sink()
{
    Pa_StopStream(d_stream); // wait for output to drain
    Pa_CloseStream(d_stream);
    Pa_Terminate();
}

/*
 * This version consumes everything sent to it, blocking if required.
 * I think this will allow us better control of the total buffering/latency
 * in the audio path.
 */
int portaudio_sink::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    const float** in = (const float**)&input_items[0];
    const unsigned nchan =
        d_output_parameters.channelCount; // # of channels == samples/frame

    int k;
    for (k = 0; k < noutput_items;) {
        int nframes = d_writer->space_available() / nchan; // How much space in ringbuffer
        if (nframes == 0) {                                // no room...
            if (d_ok_to_block) {
                {
                    gr::thread::scoped_lock guard(d_ringbuffer_mutex);
                    while (!d_ringbuffer_ready)
                        d_ringbuffer_cond.wait(guard);
                }
                continue;
            } else {
                // There's no room and we're not allowed to block.
                // (A USRP is most likely controlling the pacing through the pipeline.)
                // We drop the samples on the ground, and say we processed them all ;)
                //
                // FIXME, there's probably room for a bit more finesse here.
                return noutput_items;
            }
        }

        // We can write the smaller of the request and the room we've got
        {
            gr::thread::scoped_lock guard(d_ringbuffer_mutex);

            int nf = std::min(noutput_items - k, nframes);
            float* p = (float*)d_writer->write_pointer();

            for (int i = 0; i < nf; i++) {
                for (unsigned int c = 0; c < nchan; c++) {
                    *p++ = in[c][k + i];
                }
            }

            d_writer->update_write_pointer(nf * nchan);
            k += nf;

            d_ringbuffer_ready = false;
        }
    }

    return k; // tell how many we actually did
}

void portaudio_sink::output_error_msg(const char* msg, int err)
{
    GR_LOG_ERROR(d_logger,
                 boost::format("%s: %s %s") % d_device_name.c_str() % msg %
                     Pa_GetErrorText(err));
}

void portaudio_sink::bail(const char* msg, int err)
{
    output_error_msg(msg, err);
    throw std::runtime_error("audio_portaudio_sink");
}

} /* namespace audio */
} /* namespace gr */
