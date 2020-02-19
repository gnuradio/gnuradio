/* -*- c++ -*- */
/*
 * Copyright 2004-2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_ALSA_SOURCE_H
#define INCLUDED_AUDIO_ALSA_SOURCE_H

// use new ALSA API
#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API

#include <gnuradio/audio/source.h>
#include <alsa/asoundlib.h>
#include <stdexcept>
#include <string>

namespace gr {
namespace audio {

class alsa_source;
typedef boost::shared_ptr<alsa_source> alsa_source_sptr;

/*!
 * \brief audio source using ALSA
 * \ingroup audio_blk
 *
 * The source has between 1 and N input streams of floats, where N is
 * depends on the hardware characteristics of the selected device.
 *
 * Output samples will be in the range [-1,1].
 */
class alsa_source : public source
{
    // typedef for pointer to class work method
    typedef int (alsa_source::*work_t)(int noutput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items);

    unsigned int d_sampling_rate;
    std::string d_device_name;
    snd_pcm_t* d_pcm_handle;
    snd_pcm_hw_params_t* d_hw_params;
    snd_pcm_sw_params_t* d_sw_params;
    snd_pcm_format_t d_format;
    unsigned int d_nperiods;
    unsigned int d_period_time_us;    // microseconds
    snd_pcm_uframes_t d_period_size;  // in frames
    unsigned int d_buffer_size_bytes; // sizeof of d_buffer
    char* d_buffer;
    work_t d_worker;         // the work method to use
    unsigned int d_hw_nchan; // # of configured h/w channels
    bool d_special_case_stereo_to_mono;

    // random stats
    int d_noverruns; // count of overruns
    int d_nsuspends; // count of suspends

    void output_error_msg(const char* msg, int err);
    void bail(const char* msg, int err);

public:
    alsa_source(int sampling_rate, const std::string device_name, bool ok_to_block);
    ~alsa_source();

    bool check_topology(int ninputs, int noutputs);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

protected:
    bool read_buffer(void* buffer, unsigned nframes, unsigned sizeof_frame);

    int work_s16(int noutput_items,
                 gr_vector_const_void_star& input_items,
                 gr_vector_void_star& output_items);

    int work_s16_2x1(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    int work_s32(int noutput_items,
                 gr_vector_const_void_star& input_items,
                 gr_vector_void_star& output_items);

    int work_s32_2x1(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);
};

} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_ALSA_SOURCE_H */
