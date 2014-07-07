/* -*- c++ -*- */
/*
 * Copyright 2003,2005,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
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


#ifndef INCLUDED_GR_OSCOPE_GUTS_H
#define INCLUDED_GR_OSCOPE_GUTS_H

#include <gnuradio/wxgui/api.h>
#include <gnuradio/wxgui/trigger_mode.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace wxgui {

    /*!
     * \brief guts of oscilloscope trigger and buffer module
     *
     * This module processes sets of samples provided the \p
     * process_sample method. When appropriate given the updateRate,
     * sampleRate and trigger conditions, process_sample will
     * periodically write output records of captured data to
     * output_fd. For each trigger event, nchannels records will be
     * written. Each record consists of get_samples_per_output_record
     * binary floats. The trigger instant occurs at the 1/2 way point
     * in the buffer. Thus, output records consist of 50% pre-trigger
     * data and 50% post-trigger data.
     */

    class WXGUI_API oscope_guts
    {
    public:
      static const int MAX_CHANNELS = 8;

    private:
      enum scope_state  { HOLD_OFF, LOOK_FOR_TRIGGER, POST_TRIGGER };

      int d_nchannels;		     // how many channels
      msg_queue::sptr d_msgq;      // message queue we stuff output records into
      trigger_mode  d_trigger_mode;
      trigger_slope d_trigger_slope;
      int d_trigger_channel;         // which channel to watch for trigger condition
      double d_sample_rate;          // input sample rate in Hz
      double d_update_rate;          // approx freq to produce an output record (Hz)
      double d_trigger_level;

      int d_obi;                     // output buffer index
      float *d_buffer[MAX_CHANNELS];

      scope_state d_state;
      int d_decimator_count;
      int d_decimator_count_init;
      int d_hold_off_count;
      int d_hold_off_count_init;
      int d_pre_trigger_count;
      int d_post_trigger_count;
      int d_post_trigger_count_init;
      float d_trigger_off;          //%sample trigger is off

      // NOT IMPLEMENTED
      oscope_guts(const oscope_guts &rhs);              // no copy constructor
      oscope_guts &operator= (const oscope_guts &rhs);  // no assignment operator

      void trigger_changed();
      void update_rate_or_decimation_changed();
      bool found_trigger();          // returns true if found
      void write_output_records();

      void enter_hold_off();         // called on state entry
      void enter_look_for_trigger();
      void enter_post_trigger();

    public:
      // CREATORS
      oscope_guts(double sample_rate, msg_queue::sptr msgq);
      ~oscope_guts();

      // MANIPULATORS

      /*!
       * \param channel_data points to nchannels float values. These
       * are the values for each channel at this sample time.
       */
      void process_sample(const float *channel_data);

      bool set_update_rate(double update_rate);
      bool set_decimation_count(int decimation_count);
      bool set_trigger_channel(int channel);
      bool set_trigger_mode(trigger_mode mode);
      bool set_trigger_slope(trigger_slope slope);
      bool set_trigger_level(double trigger_level);
      bool set_trigger_level_auto();                // set to 50% level
      bool set_sample_rate(double sample_rate);
      bool set_num_channels(int nchannels);

      // ACCESSORS
      int num_channels() const;
      double sample_rate() const;
      double update_rate() const;
      int get_decimation_count() const;
      int get_trigger_channel() const;
      trigger_mode get_trigger_mode() const;
      trigger_slope get_trigger_slope() const;
      double get_trigger_level() const;

      // # of samples written to each output record.
      int get_samples_per_output_record() const;
    };

  } /* namespace wxgui */
} /* namespace gr */

#endif /* INCLUDED_GR_OSCOPE_GUTS_H */
