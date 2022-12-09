/* -*- c++ -*- */
/*
 * Copyright 2022 Volker Schroer, dl1ksv
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "../lib/alsa_internal.h"
#include <gnuradio/audio/alsa_source.h>

#include <functional>

namespace gr {
namespace audio {

class alsa_source_cpu : public virtual alsa_source
{
public:
    alsa_source_cpu(block_args args);
    work_return_t work(work_io& wio) override;

private:
    unsigned int d_sampling_rate;
    std::string d_device_name;
    alsa_internal::sndpcm_wrap d_pcm_handle;
    alsa_internal::hwparam_wrap d_hw_params;
    alsa_internal::swparam_wrap d_sw_params;
    snd_pcm_format_t d_format;
    unsigned int d_nperiods;
    unsigned int d_period_time_us;       // microseconds
    snd_pcm_uframes_t d_period_size = 0; // in frames
    std::vector<char> d_buffer;
    work_t d_worker = nullptr;   // the work method to use
    unsigned int d_hw_nchan = 0; // # of configured h/w channels
    bool d_special_case_mono_to_stereo;

    // random stats
    int d_noverruns = 0; // count of underruns
    int d_nsuspends = 0; // count of suspends
    bool d_ok_to_block;  // defaults to "true", controls blocking/non-block I/O
    size_t d_num_outputs;

    void output_error_msg(const char* msg, int err);
    void bail(const char* msg, int err);

    bool start() override;

protected:
    bool read_buffer(const void* buffer, unsigned nframes, unsigned sizeof_frame);

    work_return_t work_s16(work_io& wio);

    work_return_t work_s16_2x1(work_io& wio);

    work_return_t work_s32(work_io& wio);

    work_return_t work_s32_2x1(work_io& wio);
};

} // namespace audio
} // namespace gr
