/* -*- c++ -*- */
/*
 * Copyright 2004-2011,2013-2014 Free Software Foundation, Inc.
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
#include "windows_source.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <gnuradio/prefs.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cctype>
#include <sstream>
#include <stdexcept>

#define MAX_QUEUE_SIZE 100

namespace gr {
namespace audio {

// Currently this audio source will only support a single channel input at 16-bits.  So a
// stereo input will likely be turned into a mono by the wave mapper

source::sptr windows_source_fcn(int sampling_rate, const std::string& device_name, bool)
{
    return source::sptr(new windows_source(sampling_rate, device_name));
}

static const double CHUNK_TIME = prefs::singleton()->get_double(
    "audio_windows",
    "period_time",
    0.1); // 100 ms (below 3ms distortion will likely occur regardless of number of
          // buffers, will likely be a higher limit on slower machines)
static const int nPeriods = prefs::singleton()->get_long(
    "audio_windows",
    "nperiods",
    4); // 4 should be more than enough with a normal chunk time (2 will likely work as
        // well)... at 3ms chunks 10 was enough on a fast machine
static const bool verbose =
    prefs::singleton()->get_bool("audio_windows", "verbose", false);
static const std::string default_device =
    prefs::singleton()->get_string("audio_windows", "standard_input_device", "default");

static std::string default_device_name()
{
    return (default_device == "default" ? "WAVE_MAPPER" : default_device);
}

windows_source::windows_source(int sampling_freq, const std::string device_name)
    : sync_block("audio_windows_source",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, sizeof(float))),
      d_sampling_freq(sampling_freq),
      d_device_name(device_name.empty() ? default_device_name() : device_name),
      d_fd(-1),
      lp_buffers(0),
      d_chunk_size(0)
{
    /* Initialize the WAVEFORMATEX for 16-bit, mono */
    wave_format.wFormatTag = WAVE_FORMAT_PCM;
    wave_format.nChannels =
        1; // changing this will require adjustments to the work routine.
    wave_format.wBitsPerSample =
        16; // changing this will necessitate changing buffer type from short.
    wave_format.nSamplesPerSec =
        d_sampling_freq; // defined by flowgraph settings, but note that the microphone
                         // will likely have a native sample rate that the audio system
                         // may upsample to you desired rate, so check where the cutoff
                         // ends up or check your control panel
    wave_format.nBlockAlign = wave_format.nChannels * (wave_format.wBitsPerSample / 8);
    wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
    wave_format.cbSize = 0;

    d_chunk_size = (int)(d_sampling_freq * CHUNK_TIME); // Samples per chunk
    set_output_multiple(d_chunk_size);
    d_buffer_size =
        d_chunk_size * wave_format.nChannels *
        (wave_format.wBitsPerSample / 8); // room for 16-bit audio on one channel.

    gr::logger_ptr logger, debug_logger;
    if (open_wavein_device() < 0) {
        logger->error("open_wavein_device() failed {:s}", strerror(errno));
        throw std::runtime_error("audio_windows_source:open_wavein_device() failed");
    } else {
        d_debug_logger->info("Opened windows wavein device");
    }
    lp_buffers = new LPWAVEHDR[nPeriods];
    for (int i = 0; i < nPeriods; i++) {
        lp_buffers[i] = new WAVEHDR;
        LPWAVEHDR lp_buffer = lp_buffers[i];
        lp_buffer->dwLoops = 0L;
        lp_buffer->dwFlags = 0;
        lp_buffer->dwBufferLength = d_buffer_size;
        lp_buffer->lpData = new CHAR[d_buffer_size];
        MMRESULT w_result = waveInPrepareHeader(d_h_wavein, lp_buffer, sizeof(WAVEHDR));
        if (w_result != 0) {
            logger->error("Failed to waveInPrepareHeader {:s}", strerror(errno));
            throw std::runtime_error("open_wavein_device() failed");
        }
        waveInAddBuffer(d_h_wavein, lp_buffer, sizeof(WAVEHDR));
    }
    waveInStart(d_h_wavein);
    if (verbose) {
        d_debug_logger->info(
            "Initialized {:d} {:g} ms audio buffers, total memory used: {:0.2f} kiB",
            nPeriods,
            CHUNK_TIME * 1000,
            (d_buffer_size * nPeriods) / 1024.0);
    }
}

windows_source::~windows_source()
{
    // stop playback and set all buffers to DONE.
    waveInReset(d_h_wavein);
    // Now we can deallocate the buffers
    for (int i = 0; i < nPeriods; i++) {
        if (lp_buffers[i]->dwFlags & (WHDR_DONE | WHDR_PREPARED)) {
            waveInUnprepareHeader(d_h_wavein, lp_buffers[i], sizeof(WAVEHDR));
        } else {
        }
        delete lp_buffers[i]->lpData;
    }
    /* Free the callback Event */
    waveInClose(d_h_wavein);
    delete[] lp_buffers;
}

int windows_source::work(int noutput_items,
                         gr_vector_const_void_star& input_items,
                         gr_vector_void_star& output_items)
{
    float *f0, *f1;
    DWORD dw_items = 0;
    std::unique_lock<std::mutex> lock(buffer_queue_mutex);

    while (!buffer_queue.empty()) {
        // Pull the next incoming buffer off the queue
        LPWAVEHDR next_header = buffer_queue.front();

        // Convert and calculate the number of samples (might not be full)
        short* lp_buffer = (short*)next_header->lpData;
        DWORD buffer_length = next_header->dwBytesRecorded / sizeof(short);

        if (buffer_length + dw_items > noutput_items * output_items.size()) {
            // There's not enough output buffer space to send the whole input buffer
            // so don't try, just leave it in the queue
            // or else we'd have to track how much we sent etc
            // In theory we should never reach this code because the buffers should all be
            // sized the same
            return dw_items;
        } else {
            switch (output_items.size()) {
            case 1: // mono output
                f0 = (float*)output_items[0];

                for (int j = 0; j < buffer_length; j++) {
                    f0[dw_items + j] = (float)(lp_buffer[j]) / 32767.0;
                }
                dw_items += buffer_length;
                break;
            case 2: // stereo output (interleaved in the buffer)
                f0 = (float*)output_items[0];
                f1 = (float*)output_items[1];

                for (int j = 0; j < buffer_length / 2; j++) {
                    f0[dw_items + j] = (float)(lp_buffer[2 * j + 0]) / 32767.0;
                    f1[dw_items + j] = (float)(lp_buffer[2 * j + 1]) / 32767.0;
                }
                dw_items += buffer_length / 2;
            }
            buffer_queue.pop();
            lock.unlock();

            // Recycle the buffer
            next_header->dwFlags = 0;
            waveInPrepareHeader(d_h_wavein, next_header, sizeof(WAVEHDR));
            waveInAddBuffer(d_h_wavein, next_header, sizeof(WAVEHDR));

            lock.lock();
        }
    }
    return dw_items;
}

int windows_source::string_to_int(const std::string& s)
{
    int i;
    std::istringstream(s) >> i;
    return i;
}

MMRESULT windows_source::is_format_supported(LPWAVEFORMATEX pwfx, UINT uDeviceID)
{
    return (waveInOpen(NULL,                // ptr can be NULL for query
                       uDeviceID,           // the device identifier
                       pwfx,                // defines requested format
                       NULL,                // no callback
                       NULL,                // no instance data
                       WAVE_FORMAT_QUERY)); // query only, do not open device
}

bool windows_source::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

UINT windows_source::find_device(std::string szDeviceName)
{
    gr::logger_ptr logger, debug_logger;

    UINT result = -1;
    UINT num_devices = waveInGetNumDevs();
    if (num_devices > 0) {
        // what the device name passed as a number?
        if (is_number(szDeviceName)) {
            // a number, so must be referencing a device ID (which incremement from zero)
            UINT num = std::stoul(szDeviceName);
            if (num < num_devices) {
                result = num;
            } else {
                logger->warn("waveIn deviceID {:d} was not found, "
                             "defaulting to WAVE_MAPPER",
                             num);
                result = WAVE_MAPPER;
            }

        } else {
            // device name passed as string
            for (UINT i = 0; i < num_devices; i++) {
                WAVEINCAPS woc;
                if (waveInGetDevCaps(i, &woc, sizeof(woc)) != MMSYSERR_NOERROR) {
                    logger->error("Could not retrieve wave out device "
                                  "capabilities for device {:s}",
                                  strerror(errno));
                    return -1;
                }
                if (woc.szPname == szDeviceName) {
                    result = i;
                }
                if (verbose)
                    d_debug_logger->info("WaveIn Device {:d}: {:s}", i, woc.szPname);
            }
            if (result == -1) {
                d_debug_logger->info("Warning: waveIn device '{:s}' was not found, "
                                     "defaulting to WAVE_MAPPER",
                                     szDeviceName);
                result = WAVE_MAPPER;
            }
        }
    } else {
        logger->error("No WaveIn devices present or accessible: {:s}", strerror(errno));
    }
    return result;
}

int windows_source::open_wavein_device(void)
{
    UINT u_device_id;
    unsigned long result;

    gr::logger_ptr logger, debug_logger;

    /** Identifier of the waveform-audio output device to open. It
    can be either a device identifier or a handle of an open
    waveform-audio input device. You can use the following flag
    instead of a device identifier.
    WAVE_MAPPER The function selects a waveform-audio output
    device capable of playing the given format.
    */
    if (d_device_name.empty() || default_device_name() == d_device_name)
        u_device_id = WAVE_MAPPER;
    else
        // The below could be uncommented to allow selection of different device handles
        // however it is unclear what other devices are out there and how a user
        // would know the device ID so at the moment we will ignore that setting
        // and stick with WAVE_MAPPER
        u_device_id = find_device(d_device_name);
    if (verbose)
        d_debug_logger->info("waveIn Device ID: {:d}", u_device_id);

    // Check if the sampling rate/bits/channels are good to go with the device.
    MMRESULT supported = is_format_supported(&wave_format, u_device_id);
    if (supported != MMSYSERR_NOERROR) {
        char err_msg[50];
        waveInGetErrorText(supported, err_msg, 50);
        d_debug_logger->info("format error: {:s}", err_msg);
        logger->error("Requested audio format is not supported by device driver: {:s}",
                      strerror(errno));
        return -1;
    }

    // Open a waveform device for output using event callback.
    result = waveInOpen(&d_h_wavein,
                        u_device_id,
                        &wave_format,
                        (DWORD_PTR)&read_wavein,
                        (DWORD_PTR)this,
                        CALLBACK_FUNCTION | WAVE_ALLOWSYNC);

    if (result) {
        logger->error("Failed to open waveform output device: {:s}", strerror(errno));
        return -1;
    }
    return 0;
}

static void CALLBACK read_wavein(
    HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    // Ignore WIM_OPEN and WIM_CLOSE messages
    if (uMsg == WIM_DATA) {
        if (!dwInstance) {
            gr::logger_ptr logger;
            logger->error("callback function missing buffer queue: {:s}",
                          strerror(errno));
        }
        LPWAVEHDR lp_wave_hdr = (LPWAVEHDR)dwParam1; // The new audio data
        windows_source* source = (windows_source*)dwInstance;
        {
            std::lock_guard<std::mutex> lock(source->buffer_queue_mutex);
            if (source->buffer_queue.size() < MAX_QUEUE_SIZE) {
                source->buffer_queue.push(lp_wave_hdr); // Add the buffer to the queue
            }
        }
    }
}
} /* namespace audio */
} /* namespace gr */
