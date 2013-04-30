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

#ifndef INCLUDED_GR_OSCOPE_SINK_X_H
#define INCLUDED_GR_OSCOPE_SINK_X_H

#include <gnuradio/wxgui/api.h>
#include <gnuradio/wxgui/trigger_mode.h>
#include <gnuradio/wxgui/oscope_guts.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace wxgui {

    /*!
     * \brief Abstract class for python oscilloscope module.
     * \ingroup sink_blk
     *
     * Don't instantiate this. Use gr::blocks::oscope_sink_f instead.
     */
    class WXGUI_API oscope_sink_x : public sync_block
    {
    protected:
      double d_sampling_rate;
      oscope_guts *d_guts;

      oscope_sink_x() {};
      oscope_sink_x(const std::string name,
                    gr::io_signature::sptr input_sig,
                    double sampling_rate);
    public:
      virtual ~oscope_sink_x();

      //// gr::blocks::oscope_sink_x::sptr
      //typedef boost::shared_ptr<oscope_sink_x> sptr;
      //
      //static sptr make(const std::string name,
      //                 gnuradio/io_signature.h_sptr input_sig,
      //                 double sampling_rate);

      bool set_update_rate(double update_rate);
      bool set_decimation_count(int decimation_count);
      bool set_trigger_channel(int channel);
      bool set_trigger_mode(trigger_mode mode);
      bool set_trigger_slope(trigger_slope slope);
      bool set_trigger_level(double trigger_level);
      bool set_trigger_level_auto();
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

      virtual int work(int noutput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items) = 0;
    };

  } /* namespace wxgui */
} /* namespace gr */

#endif /* INCLUDED_GR_OSCOPE_SINK_X_H */
