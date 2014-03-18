/* -*- c++ -*- */
/*
 * Copyright 2006-2011,2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_AUDIO_OSX_SOURCE_H
#define INCLUDED_AUDIO_OSX_SOURCE_H

#include <gnuradio/audio/source.h>

#include "osx_common.h"
#include "circular_buffer.h"

namespace gr {
  namespace audio {

    /*!
     * \brief audio source using OSX
     * \ingroup audio_blk
     *
     * Input signature is one or two streams of floats.
     * Samples must be in the range [-1,1].
     */
    class osx_source : public source
    {
    private:

      Float64 d_device_sample_rate, d_output_sample_rate;
      UInt32 d_input_buffer_size_frames, d_input_buffer_size_bytes;
      UInt32 d_output_buffer_size_frames, d_output_buffer_size_bytes;
      UInt32 d_device_buffer_size_frames, d_device_buffer_size_bytes;
      UInt32 d_lead_size_frames, d_lead_size_bytes;
      UInt32 d_trail_size_frames, d_trail_size_bytes;
      UInt32 d_extra_buffer_size_frames, d_extra_buffer_size_bytes;
      UInt32 d_queue_sample_count, d_buffer_sample_count;
      UInt32 d_n_available_input_frames, d_n_actual_input_frames;
      UInt32 d_n_user_channels, d_n_dev_channels;
      bool d_ok_to_block, d_pass_through;
      bool d_waiting_for_data, d_do_reset, d_hardware_changed;
      bool d_using_default_device;
      gr::thread::mutex d_internal;
      gr::thread::condition_variable d_cond_data;
      std::vector < circular_buffer<float>* > d_buffers;
      std::string d_desired_name, d_selected_name;

      // CoreAudio variables

      AudioDeviceID       d_input_ad_id;
      AudioUnit           d_input_au;
      AudioBufferList*    d_input_buffer;
      AudioBufferList*    d_output_buffer;
      AudioConverterRef   d_audio_converter;

      // d_asbd_device: ASBD of the device that is creating the input
      // data stream

      AudioStreamBasicDescription d_asbd_device;

      // d_asbd_client: ASBD of the client side (output) of the
      // hardware device

      AudioStreamBasicDescription d_asbd_client;

      // d_asbd_user: ASBD of the user's arguments, if an audio
      // converter is needed outside that provided by the client side.

      AudioStreamBasicDescription d_asbd_user;

    public:

      osx_source(int sample_rate,
                 const std::string& device_name,
                 bool ok_to_block);

      virtual inline ~osx_source() {
	teardown();
      }

      virtual bool start();
      virtual bool stop();

      virtual bool check_topology(int ninputs, int noutputs);

      virtual int work(int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);

      inline void reset(bool hardware_changed) {
	d_hardware_changed = hardware_changed;
	d_do_reset = true;
      }

    private:

      bool is_running();

      void setup();

      void teardown();

      void alloc_audio_buffer_list
      (AudioBufferList** t_abl,
       UInt32 n_channels,
       UInt32 input_buffer_size_bytes);

      void free_audio_buffer_list
      (AudioBufferList** t_abl);

      static OSStatus converter_callback
      (AudioConverterRef in_audio_converter,
       UInt32* io_number_data_packets,
       AudioBufferList* io_data,
       AudioStreamPacketDescription** out_aspd,
       void* in_user_data);

      static OSStatus au_input_callback
      (void *in_ref_con,
       AudioUnitRenderActionFlags *io_action_flags,
       const AudioTimeStamp *in_time_stamp,
       UInt32 in_bus_number,
       UInt32 in_number_frames,
       AudioBufferList *io_data);

#ifndef GR_USE_OLD_AUDIO_UNIT

      // OSX 10.4 and newer

      static OSStatus hardware_listener
      (AudioObjectID in_object_id,
       UInt32 in_num_addresses,
       const AudioObjectPropertyAddress in_addresses[],
       void* in_client_data);

      static OSStatus default_listener
      (AudioObjectID in_object_id,
       UInt32 in_num_addresses,
       const AudioObjectPropertyAddress in_addresses[],
       void* in_client_data);

#else

      // OSX 10.6 and older; removed as of 10.7

      static OSStatus hardware_listener
      (AudioHardwarePropertyID in_property_id,
       void* in_client_data);

      static OSStatus default_listener
      (AudioHardwarePropertyID in_property_id,
       void* in_client_data);

#endif

    };
  } /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_OSX_SOURCE_H */
