/* -*- c++ -*- */
/*
 * Copyright 2004-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_WINDOWS_SOURCE_H
#define INCLUDED_AUDIO_WINDOWS_SOURCE_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX // stops windef.h defining max/min under cygwin

#include <mmsystem.h>
#include <windows.h>

#include <gnuradio/audio/source.h>
#include <mutex>
#include <queue>
#include <string>

namespace gr {
namespace audio {

/*!
 * \brief audio source using winmm mmsystem (win32 only)
 * \ingroup audio_blk
 *
 * Output signature is one or two streams of floats.
 * Output samples will be in the range [-1,1].
 */
class windows_source : public source
{
    int d_sampling_freq;
    std::string d_device_name;
    int d_fd;
    LPWAVEHDR* lp_buffers;
    DWORD d_chunk_size;
    DWORD d_buffer_size;
    HWAVEIN d_h_wavein;
    WAVEFORMATEX wave_format;

protected:
    int string_to_int(const std::string& s);
    int open_wavein_device(void);
    MMRESULT is_format_supported(LPWAVEFORMATEX pwfx, UINT uDeviceID);
    bool is_number(const std::string& s);
    UINT find_device(std::string szDeviceName);
    std::queue<LPWAVEHDR> buffer_queue;
    std::mutex buffer_queue_mutex;
    friend static void CALLBACK read_wavein(HWAVEIN hwi,
                                            UINT uMsg,
                                            DWORD_PTR dwInstance,
                                            DWORD_PTR dwParam1,
                                            DWORD_PTR dwParam2);

public:
    windows_source(int sampling_freq, const std::string device_name = "");
    ~windows_source();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

static void CALLBACK read_wavein(
    HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_WINDOWS_SOURCE_H */
