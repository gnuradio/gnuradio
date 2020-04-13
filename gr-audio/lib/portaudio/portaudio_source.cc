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
#include "portaudio_source.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#ifdef _MSC_VER
#include <io.h>
#endif

namespace gr {
namespace audio {

source::sptr
portaudio_source_fcn(int sampling_rate, const std::string& device_name, bool ok_to_block)
{
    return source::sptr(new portaudio_source(sampling_rate, device_name, ok_to_block));
}

//#define LOGGING 0  // define to 0 or 1

#define SAMPLE_FORMAT paFloat32

typedef float sample_t;

// Number of portaudio buffers in the ringbuffer
static const unsigned int N_BUFFERS = 4;

static std::string default_device_name()
{
    return prefs::singleton()->get_string("audio_portaudio", "default_input_device", "");
}

void portaudio_source::create_ringbuffer(void)
{
    int bufsize_samples =
        d_portaudio_buffer_size_frames * d_input_parameters.channelCount;

    if (d_verbose)
        GR_LOG_INFO(d_debug_logger,
                    boost::format("ring buffer size  = %d frames") %
                        (N_BUFFERS * bufsize_samples / d_input_parameters.channelCount));

    // FYI, the buffer indices are in units of samples.
    d_writer = gr::make_buffer(N_BUFFERS * bufsize_samples, sizeof(sample_t));
    d_reader = gr::buffer_add_reader(d_writer, 0);
}

/*
 * This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 *
 * Our job is to copy framesPerBuffer frames from inputBuffer.
 */
int portaudio_source_callback(const void* inputBuffer,
                              void* outputBuffer,
                              unsigned long framesPerBuffer,
                              const PaStreamCallbackTimeInfo* timeInfo,
                              PaStreamCallbackFlags statusFlags,
                              void* arg)
{
    portaudio_source* self = (portaudio_source*)arg;
    int nchan = self->d_input_parameters.channelCount;
    int nframes_to_copy = framesPerBuffer;
    int nframes_room = self->d_writer->space_available() / nchan;

    if (nframes_to_copy <= nframes_room) { // We've got room for the data ..
        // if (LOGGING)
        //  self->d_log->printf("PAsrc  cb: f/b = %4ld\n", framesPerBuffer);

        // copy from input buffer to ringbuffer
        {
            gr::thread::scoped_lock guard(self->d_ringbuffer_mutex);

            memcpy(self->d_writer->write_pointer(),
                   inputBuffer,
                   nframes_to_copy * nchan * sizeof(sample_t));
            self->d_writer->update_write_pointer(nframes_to_copy * nchan);

            // Tell the source thread there is new data in the ringbuffer.
            self->d_ringbuffer_ready = true;
        }

        self->d_ringbuffer_cond.notify_one();
        return paContinue;
    }

    else { // overrun
        self->d_noverruns++;
        ssize_t r = ::write(2, "aO", 2); // FIXME change to non-blocking call
        if (r == -1) {
            gr::logger_ptr logger, debug_logger;
            gr::configure_default_loggers(
                logger, debug_logger, "portaudio_source_callback");
            GR_LOG_ERROR(logger, boost::format("write error: %s") % strerror(errno));
        }

        self->d_ringbuffer_ready = false;
        self->d_ringbuffer_cond.notify_one(); // Tell the sink to get going!
        return paContinue;
    }
}

// ----------------------------------------------------------------

portaudio_source::portaudio_source(int sampling_rate,
                                   const std::string device_name,
                                   bool ok_to_block)
    : sync_block("audio_portaudio_source",
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
      d_ringbuffer_ready(false),
      d_noverruns(0)
{
    memset(&d_input_parameters, 0, sizeof(d_input_parameters));

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
        device = Pa_GetDefaultInputDevice();
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
            if (deviceInfo->maxInputChannels <= 0) {
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
        }
    }

    d_input_parameters.device = device;
    d_input_parameters.channelCount = deviceInfo->maxInputChannels;
    d_input_parameters.sampleFormat = SAMPLE_FORMAT;
    d_input_parameters.suggestedLatency = deviceInfo->defaultLowInputLatency;
    d_input_parameters.hostApiSpecificStreamInfo = NULL;

    // We fill in the real channelCount in check_topology when we know
    // how many inputs are connected to us.

    // Now that we know the maximum number of channels (allegedly)
    // supported by the h/w, we can compute a reasonable output
    // signature.  The portaudio specs say that they'll accept any
    // number of channels from 1 to max.
    set_output_signature(
        io_signature::make(1, deviceInfo->maxInputChannels, sizeof(sample_t)));
}

bool portaudio_source::check_topology(int ninputs, int noutputs)
{
    PaError err;

    if (Pa_IsStreamActive(d_stream)) {
        Pa_CloseStream(d_stream);
        d_stream = 0;
        d_reader.reset(); // std::shared_ptr for d_reader = 0
        d_writer.reset(); // std::shared_ptr for d_write = 0
    }

    d_input_parameters.channelCount = noutputs; // # of channels we're really using

#if 1
    d_portaudio_buffer_size_frames =
        (int)(0.0213333333 * d_sampling_rate + 0.5); // Force 512 frame buffers at 48000
    GR_LOG_ERROR(d_logger,
                 boost::format("Latency = %8.5f, requested sampling_rate = %g") %
                     0.0213333333 % (double)d_sampling_rate);
#endif
    err = Pa_OpenStream(&d_stream,
                        &d_input_parameters,
                        NULL, // No output
                        d_sampling_rate,
                        d_portaudio_buffer_size_frames,
                        paClipOff,
                        &portaudio_source_callback,
                        (void*)this);

    if (err != paNoError) {
        output_error_msg("OpenStream failed", err);
        return false;
    }

#if 0
        const PaStreamInfo *psi = Pa_GetStreamInfo(d_stream);

        d_portaudio_buffer_size_frames = (int)(d_input_parameters.suggestedLatency  * psi->sampleRate);
        GR_LOG_ERROR(d_logger,
                     boost::format("Latency = %7.4f, psi->sampleRate = %g") %
                     d_input_parameters.suggestedLatency % psi->sampleRate);
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

portaudio_source::~portaudio_source()
{
    Pa_StopStream(d_stream); // wait for output to drain
    Pa_CloseStream(d_stream);
    Pa_Terminate();
}

int portaudio_source::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    float** out = (float**)&output_items[0];
    const unsigned nchan =
        d_input_parameters.channelCount; // # of channels == samples/frame

    int k;
    for (k = 0; k < noutput_items;) {
        int nframes = d_reader->items_available() / nchan; // # of frames in ringbuffer
        if (nframes == 0) {                                // no data right now...
            if (k > 0) // If we've produced anything so far, return that
                return k;

            if (d_ok_to_block) {
                gr::thread::scoped_lock guard(d_ringbuffer_mutex);
                while (d_ringbuffer_ready == false)
                    d_ringbuffer_cond.wait(guard); // block here, then try again
                continue;
            }

            assert(k == 0);

            // There's no data and we're not allowed to block.
            // (A USRP is most likely controlling the pacing through the pipeline.)
            // This is an underrun.  The scheduler wouldn't have called us if it
            // had anything better to do.  Thus we really need to produce some amount
            // of "fill".
            //
            // There are lots of options for comfort noise, etc.
            // FIXME We'll fill with zeros for now.  Yes, it will "click"...

            // Fill with some frames of zeros
            {
                gr::thread::scoped_lock guard(d_ringbuffer_mutex);

                int nf = std::min(noutput_items - k, (int)d_portaudio_buffer_size_frames);
                for (int i = 0; i < nf; i++) {
                    for (unsigned int c = 0; c < nchan; c++) {
                        out[c][k + i] = 0;
                    }
                }
                k += nf;

                d_ringbuffer_ready = false;
                return k;
            }
        }

        // We can read the smaller of the request and what's in the buffer.
        {
            gr::thread::scoped_lock guard(d_ringbuffer_mutex);

            int nf = std::min(noutput_items - k, nframes);

            const float* p = (const float*)d_reader->read_pointer();
            for (int i = 0; i < nf; i++) {
                for (unsigned int c = 0; c < nchan; c++) {
                    out[c][k + i] = *p++;
                }
            }
            d_reader->update_read_pointer(nf * nchan);
            k += nf;
            d_ringbuffer_ready = false;
        }
    }

    return k; // tell how many we actually did
}

void portaudio_source::output_error_msg(const char* msg, int err)
{
    GR_LOG_ERROR(d_logger,
                 boost::format("%s: %s %s") % d_device_name.c_str() % msg %
                     Pa_GetErrorText(err));
}

void portaudio_source::bail(const char* msg, int err)
{
    output_error_msg(msg, err);
    throw std::runtime_error("audio_portaudio_source");
}

} /* namespace audio */
} /* namespace gr */
