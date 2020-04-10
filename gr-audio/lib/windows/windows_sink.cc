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
#include "windows_sink.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <gnuradio/prefs.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace gr {
namespace audio {

sink::sptr
windows_sink_fcn(int sampling_rate, const std::string& device_name, bool ok_to_block)
{
    return sink::sptr(new windows_sink(sampling_rate, device_name, ok_to_block));
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
    prefs::singleton()->get_string("audio_windows", "standard_output_device", "default");

static std::string default_device_name()
{
    return (default_device == "default" ? "WAVE_MAPPER" : default_device);
}

windows_sink::windows_sink(int sampling_freq,
                           const std::string device_name,
                           bool ok_to_block)
    : sync_block("audio_windows_sink",
                 io_signature::make(1, 2, sizeof(float)),
                 io_signature::make(0, 0, 0)),
      d_sampling_freq(sampling_freq),
      d_device_name(device_name.empty() ? default_device_name() : device_name),
      d_fd(-1),
      d_buffers(0),
      d_chunk_size(0),
      d_ok_to_block(ok_to_block)
{
    /* Initialize the WAVEFORMATEX for 16-bit, 44KHz, stereo */
    wave_format.wFormatTag = WAVE_FORMAT_PCM;
    wave_format.nChannels =
        2; // changing this will require adjustments to the work routine.
    wave_format.wBitsPerSample =
        16; // changing this will necessitate changing buffer type from short.
    wave_format.nSamplesPerSec =
        d_sampling_freq; // 44100 is default but up to flowgraph settings;
    wave_format.nBlockAlign = wave_format.nChannels * (wave_format.wBitsPerSample / 8);
    wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
    wave_format.cbSize = 0;

    d_chunk_size = (int)(d_sampling_freq * CHUNK_TIME); // Samples per chunk
    set_output_multiple(d_chunk_size);
    d_buffer_size =
        d_chunk_size * wave_format.nChannels *
        (wave_format.wBitsPerSample / 8); // room for 16-bit audio on two channels.

    d_wave_write_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (open_waveout_device() < 0) {
        GR_LOG_ERROR(d_logger,
                     boost::format("open_waveout_device() failed: %s") % strerror(errno));
        throw std::runtime_error("audio_windows_sink:open_waveout_device() failed");
    } else {
        GR_LOG_INFO(d_debug_logger, "Opened windows waveout device");
    }
    d_buffers = new LPWAVEHDR[nPeriods];
    for (int i = 0; i < nPeriods; i++) {
        d_buffers[i] = new WAVEHDR;
        d_buffers[i]->dwLoops = 0L;
        d_buffers[i]->dwFlags = WHDR_DONE;
        d_buffers[i]->dwBufferLength = d_buffer_size;
        d_buffers[i]->lpData = new CHAR[d_buffer_size];
    }
    GR_LOG_INFO(
        d_debug_logger,
        boost::format(
            "Initialized %1% %2% ms audio buffers, total memory used: %3$0.2f kB") %
            (nPeriods) % (CHUNK_TIME * 1000) % ((d_buffer_size * nPeriods) / 1024.0));
}

windows_sink::~windows_sink()
{
    // stop playback and set all buffers to DONE.
    waveOutReset(d_h_waveout);
    // Now we can deallocate the buffers
    for (int i = 0; i < nPeriods; i++) {
        if (d_buffers[i]->dwFlags & (WHDR_DONE | WHDR_PREPARED)) {
            waveOutUnprepareHeader(d_h_waveout, d_buffers[i], sizeof(d_buffers[i]));
        } else {
        }
        delete d_buffers[i]->lpData;
    }
    /* Free the callback Event */
    CloseHandle(d_wave_write_event);
    waveOutClose(d_h_waveout);
    delete[] d_buffers;
}

int windows_sink::work(int noutput_items,
                       gr_vector_const_void_star& input_items,
                       gr_vector_void_star& output_items)
{
    const float *f0, *f1;

    int samples_sent = 0;
    int samples_tosend = 0;
    switch (input_items.size()) {
    case 1: // mono input
        f0 = (const float*)input_items[0];
        break;
    case 2: // stereo input
        f0 = (const float*)input_items[0];
        f1 = (const float*)input_items[1];
        break;
    }

    while (samples_sent < noutput_items) {
        // Pick the first available wave header (buffer)
        // If none available, then wait until the processing event if fired and check
        // again Not all events free up a buffer, so it could take more than one loop to
        // get one however, to avoid a lock, only wait 1 second for a freed up buffer then
        // abort.
        LPWAVEHDR chosen_header = NULL;
        int c = 0;
        while (!chosen_header) {
            ResetEvent(d_wave_write_event);
            for (int i = 0; i < nPeriods; i++) {
                if (d_buffers[i]->dwFlags & WHDR_DONE) {
                    // uncomment the below to see which buffers are being consumed
                    // printf("%d ", i);
                    chosen_header = d_buffers[i];
                    break;
                }
            }
            if (!chosen_header) {
                if (!d_ok_to_block) {
                    // drop the input data, print warning, and return control.
                    printf("aO");
                    return noutput_items;
                } else {
                    WaitForSingleObject(d_wave_write_event, 100);
                }
            }
            if (c++ > 10) {
                // After waiting for 1 second, then something else is seriously wrong so
                // let's just fail and give some debugging information about the status of
                // the buffers.
                for (int i = 0; i < nPeriods; i++) {
                    GR_LOG_ERROR(
                        d_logger, "audio buffer %d: %d", i, d_buffers[i]->dwFlags);
                }
                GR_LOG_ERROR(d_logger,
                             boost::format("no audio buffers available: %s") %
                                 strerror(errno));
                return -1;
            }
        }

        short* d_buffer = (short*)chosen_header->lpData;
        samples_tosend = noutput_items - samples_sent >= d_chunk_size
                             ? d_chunk_size
                             : noutput_items - samples_sent;


        switch (input_items.size()) {
        case 1: // mono input
            for (int j = 0; j < samples_tosend; j++) {
                d_buffer[2 * j + 0] = (short)(f0[j] * 32767);
                d_buffer[2 * j + 1] = (short)(f0[j] * 32767);
            }
            f0 += samples_tosend;
            break;
        case 2: // stereo input
            for (int j = 0; j < samples_tosend; j++) {
                d_buffer[2 * j + 0] = (short)(f0[j] * 32767);
                d_buffer[2 * j + 1] = (short)(f1[j] * 32767);
            }
            f0 += samples_tosend;
            f1 += samples_tosend;
            break;
        }
        if (write_waveout(chosen_header) < 0) {
            GR_LOG_ERROR(d_logger, boost::format("write failed: %s") % strerror(errno));
        }
        samples_sent += samples_tosend;
    }
    return samples_sent;
}

int windows_sink::string_to_int(const std::string& s)
{
    int i;
    std::istringstream(s) >> i;
    return i;
}

MMRESULT windows_sink::is_format_supported(LPWAVEFORMATEX pwfx, UINT uDeviceID)
{
    return (waveOutOpen(NULL,                // ptr can be NULL for query
                        uDeviceID,           // the device identifier
                        pwfx,                // defines requested format
                        NULL,                // no callback
                        NULL,                // no instance data
                        WAVE_FORMAT_QUERY)); // query only, do not open device
}

bool windows_sink::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

UINT windows_sink::find_device(std::string szDeviceName)
{
    UINT result = -1;
    UINT num_devices = waveOutGetNumDevs();
    if (num_devices > 0) {
        // what the device name passed as a number?
        if (is_number(szDeviceName)) {
            // a number, so must be referencing a device ID (which incremement from zero)
            UINT num = std::stoul(szDeviceName);
            if (num < num_devices) {
                result = num;
            } else {
                GR_LOG_WARN(d_logger,
                            boost::format("waveOut deviceID %d was not found. "
                                          "defaulting to WAVE_MAPPER") %
                                num);
                result = WAVE_MAPPER;
            }

        } else {
            // device name passed as string
            for (UINT i = 0; i < num_devices; i++) {
                WAVEOUTCAPS woc;
                if (waveOutGetDevCaps(i, &woc, sizeof(woc)) != MMSYSERR_NOERROR) {
                    GR_LOG_ERROR(d_logger,
                                 boost::format("Could not retrieve wave out device "
                                               "capabilities for %s device") %
                                     strerror(errno));
                    return -1;
                }
                if (woc.szPname == szDeviceName) {
                    result = i;
                }
                if (verbose) {
                    GR_LOG_INFO(d_debug_logger,
                                boost::format("WaveOut Device %d: %s") % i % woc.szPname);
                }
            }
            if (result == -1) {
                GR_LOG_WARN(d_logger,
                            boost::format("waveOut device '%s' was not found, "
                                          "defaulting to WAVE_MAPPER") %
                                szDeviceName);
                result = WAVE_MAPPER;
            }
        }
    } else {
        GR_LOG_ERROR(d_logger,
                     boost::format("No WaveOut devices present or accessible: %s") %
                         strerror(errno));
    }
    return result;
}

int windows_sink::open_waveout_device(void)
{
    UINT u_device_id;
    unsigned long result;

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
        GR_LOG_INFO(d_debug_logger,
                    boost::format("waveOut Device ID: %1%") % (u_device_id));

    // Check if the sampling rate/bits/channels are good to go with the device.
    MMRESULT supported = is_format_supported(&wave_format, u_device_id);
    if (supported != MMSYSERR_NOERROR) {
        char err_msg[50];
        waveOutGetErrorText(supported, err_msg, 50);
        GR_LOG_INFO(d_debug_logger, boost::format("format error: %s") % err_msg);
        GR_LOG_ERROR(
            d_logger,
            boost::format("Requested audio format is not supported by device %s driver") %
                strerror(errno));
        return -1;
    }

    // Open a waveform device for output using event callback.
    result = waveOutOpen(&d_h_waveout,
                         u_device_id,
                         &wave_format,
                         (DWORD_PTR)d_wave_write_event,
                         0,
                         CALLBACK_EVENT | WAVE_ALLOWSYNC);

    if (result)
        GR_LOG_ERROR(d_logger,
                     boost::format("Failed to open waveform output device. %s") %
                         strerr(errno));
    return -1;
    return 0;
}

int windows_sink::write_waveout(LPWAVEHDR lp_wave_hdr)
{
    UINT w_result;

    /* Clear the WHDR_DONE bit (which the driver set last time that
    this WAVEHDR was sent via waveOutWrite and was played). Some
    drivers need this to be cleared */
    lp_wave_hdr->dwFlags = 0L;

    w_result = waveOutPrepareHeader(d_h_waveout, lp_wave_hdr, sizeof(WAVEHDR));
    if (w_result != 0) {
        GR_LOG_ERROR(d_logger,
                     boost::format("Failed to waveOutPrepareHeader %s") % strerr(errno));
        return -1;
    }

    w_result = waveOutWrite(d_h_waveout, lp_wave_hdr, sizeof(WAVEHDR));
    if (w_result != 0) {
        GR_LOG_ERROR(d_logger,
                     boost::format("Failed to write block to device %s") % strerr(errno));
        switch (w_result) {
        case MMSYSERR_INVALHANDLE:
            GR_LOG_ERROR(d_logger, "Specified device handle is invalid");
            break;
        case MMSYSERR_NODRIVER:
            GR_LOG_ERROR(d_logger, "No device driver is present");
            break;
        case MMSYSERR_NOMEM:
            GR_LOG_ERROR(d_logger, "Unable to allocate or lock memory");
            break;
        case WAVERR_UNPREPARED:
            GR_LOG_ERROR(d_logger,
                         "The data block pointed to by the pwh parameter hasn't "
                         "been prepared.");
            break;
        default:
            GR_LOG_ERROR(d_logger, boost::format("Unknown error %i") % w_result);
        }
        waveOutUnprepareHeader(d_h_waveout, lp_wave_hdr, sizeof(WAVEHDR));
        return -1;
    }
    return 0;
}
} /* namespace audio */
} /* namespace gr */
