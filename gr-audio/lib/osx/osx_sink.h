/* -*- c++ -*- */
/*
 * Copyright 2006-2011,2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_OSX_SINK_H
#define INCLUDED_AUDIO_OSX_SINK_H

#include <gnuradio/audio/sink.h>

#include "circular_buffer.h"
#include "osx_common.h"

namespace gr {
namespace audio {

/*!
 * \brief audio sink using OSX
 * \ingroup audio_blk
 *
 * input signature is one or two streams of floats.
 * Input samples must be in the range [-1,1].
 */

class osx_sink : public sink
{
protected:
    Float64 d_input_sample_rate;
    UInt32 d_n_user_channels, d_n_dev_channels, d_n_buffer_channels;
    UInt32 d_queue_sample_count, d_buffer_size_samples;
    bool d_ok_to_block, d_do_reset, d_hardware_changed;
    bool d_using_default_device, d_waiting_for_data;
    gr::thread::mutex d_internal;
    gr::thread::condition_variable d_cond_data;
    std::vector<circular_buffer<float>*> d_buffers;
    std::string d_desired_name, d_selected_name;

    // AudioUnits and Such

    AudioUnit d_output_au;
    AudioDeviceID d_output_ad_id;
    AudioStreamBasicDescription d_stream_format;

public:
    osx_sink(int sample_rate, const std::string& device_name, bool ok_to_block);

    inline virtual ~osx_sink() { teardown(); }

    virtual bool check_topology(int ninputs, int noutputs);
    virtual bool start();
    virtual bool stop();

    virtual int work(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    inline void reset(bool hardware_changed)
    {
        d_hardware_changed = hardware_changed;
        d_do_reset = true;
    }

private:
    bool is_running();

    void setup();

    void teardown();

    void check_channels(bool force_reset);

    static OSStatus au_output_callback(void* in_ref_con,
                                       AudioUnitRenderActionFlags* io_action_flags,
                                       const AudioTimeStamp* in_time_stamp,
                                       UInt32 in_bus_number,
                                       UInt32 in_number_frames,
                                       AudioBufferList* io_data);

#ifndef GR_USE_OLD_AUDIO_UNIT

    // OSX 10.4 and newer

    static OSStatus hardware_listener(AudioObjectID in_object_id,
                                      UInt32 in_num_addresses,
                                      const AudioObjectPropertyAddress in_addresses[],
                                      void* in_client_data);

    static OSStatus default_listener(AudioObjectID in_object_id,
                                     UInt32 in_num_addresses,
                                     const AudioObjectPropertyAddress in_addresses[],
                                     void* in_client_data);

#else

    // OSX 10.6 and older; removed as of 10.7

    static OSStatus hardware_listener(AudioHardwarePropertyID in_property_id,
                                      void* in_client_data);

    static OSStatus default_listener(AudioHardwarePropertyID in_property_id,
                                     void* in_client_data);

#endif
};
} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_OSX_SINK_H */
