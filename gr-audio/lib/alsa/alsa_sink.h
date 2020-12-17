/* -*- c++ -*- */
/*
 * Copyright 2004-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_ALSA_SINK_H
#define INCLUDED_AUDIO_ALSA_SINK_H

// use new ALSA API
#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API

#include "alsa_impl.h"
#include <gnuradio/audio/sink.h>
#include <alsa/asoundlib.h>
#include <stdexcept>
#include <string>

namespace gr {
namespace audio {

/*!
 * \brief audio sink using ALSA
 * \ingroup audio_blk
 *
 * The sink has N input streams of floats, where N depends
 * on the hardware characteristics of the selected device.
 *
 * Input samples must be in the range [-1,1].
 */
class alsa_sink : public sink
{
    // typedef for pointer to class work method
    typedef int (alsa_sink::*work_t)(int noutput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items);

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
    work_t d_worker = 0; // the work method to use
    bool d_special_case_mono_to_stereo;

    // random stats
    int d_nunderuns = 0; // count of underruns
    int d_nsuspends = 0; // count of suspends
    bool d_ok_to_block;  // defaults to "true", controls blocking/non-block I/O

    void output_error_msg(const char* msg, int err);
    void bail(const char* msg, int err);

public:
    alsa_sink(int sampling_rate, const std::string device_name, bool ok_to_block);
    ~alsa_sink() override;

    bool check_topology(int ninputs, int noutputs) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

protected:
    bool write_buffer(const void* buffer, unsigned nframes, unsigned sizeof_frame);

    int work_s16(int noutput_items,
                 gr_vector_const_void_star& input_items,
                 gr_vector_void_star& output_items);

    int work_s16_1x2(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    int work_s32(int noutput_items,
                 gr_vector_const_void_star& input_items,
                 gr_vector_void_star& output_items);

    int work_s32_1x2(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_ALSA_SINK_H */
