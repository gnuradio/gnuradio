/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2013 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/wxgui/oscope_sink_x.h>
#include <gnuradio/wxgui/oscope_guts.h>
#include <gnuradio/io_signature.h>

namespace gr {
  namespace wxgui {

    oscope_sink_x::oscope_sink_x(const std::string name,
                                 gr::io_signature::sptr input_sig,
                                 double sampling_rate)
      : sync_block(name, input_sig,
                      io_signature::make(0, 0, 0)),
        d_sampling_rate(sampling_rate), d_guts(0)
    {
    }

    oscope_sink_x::~oscope_sink_x()
    {
      delete d_guts;
    }

    // ----------------------------------------------------------------

    bool
    oscope_sink_x::set_update_rate(double update_rate)
    {
      return d_guts->set_update_rate(update_rate);
    }

    bool
    oscope_sink_x::set_decimation_count(int decimation_count)
    {
      return d_guts->set_decimation_count(decimation_count);
    }

    bool
    oscope_sink_x::set_trigger_channel(int channel)
    {
      return d_guts->set_trigger_channel(channel);
    }

    bool
    oscope_sink_x::set_trigger_mode(trigger_mode mode)
    {
      return d_guts->set_trigger_mode(mode);
    }

    bool
    oscope_sink_x::set_trigger_slope(trigger_slope slope)
    {
      return d_guts->set_trigger_slope(slope);
    }

    bool
    oscope_sink_x::set_trigger_level(double trigger_level)
    {
      return d_guts->set_trigger_level(trigger_level);
    }

    bool
    oscope_sink_x::set_trigger_level_auto()
    {
      return d_guts->set_trigger_level_auto();
    }

    bool
    oscope_sink_x::set_sample_rate(double sample_rate)
    {
      return d_guts->set_sample_rate(sample_rate);
    }

    bool
    oscope_sink_x::set_num_channels(int nchannels)
    {
      return d_guts->set_num_channels(nchannels);
    }

    // ACCESSORS

    int
    oscope_sink_x::num_channels() const
    {
      return d_guts->num_channels();
    }

    double
    oscope_sink_x::sample_rate() const
    {
      return d_guts->sample_rate();
    }

    double
    oscope_sink_x::update_rate() const
    {
      return d_guts->update_rate();
    }

    int
    oscope_sink_x::get_decimation_count() const
    {
      return d_guts->get_decimation_count();
    }

    int
    oscope_sink_x::get_trigger_channel() const
    {
      return d_guts->get_trigger_channel();
    }

    trigger_mode
    oscope_sink_x::get_trigger_mode() const
    {
      return d_guts->get_trigger_mode();
    }

    trigger_slope
    oscope_sink_x::get_trigger_slope() const
    {
      return d_guts->get_trigger_slope();
    }

    double
    oscope_sink_x::get_trigger_level() const
    {
      return d_guts->get_trigger_level();
    }

    int
    oscope_sink_x::get_samples_per_output_record() const
    {
      return d_guts->get_samples_per_output_record();
    }

  } /* namespace wxgui */
} /* namespace gr */
