/* -*- c++ -*- */
/*
 * Copyright 2011-2013,2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_TIME_SINK_F_IMPL_H
#define INCLUDED_QTGUI_TIME_SINK_F_IMPL_H

#include <gnuradio/qtgui/time_sink_f.h>
#include <gnuradio/qtgui/timedisplayform.h>
#include <gnuradio/high_res_timer.h>

namespace gr {
  namespace qtgui {

    class QTGUI_API time_sink_f_impl : public time_sink_f
    {
    private:
      void initialize();

      int d_size, d_buffer_size;
      double d_samp_rate;
      std::string d_name;
      int d_nconnections;

      int d_index, d_start, d_end;
      std::vector<float*> d_fbuffers;
      std::vector<double*> d_buffers;
      std::vector< std::vector<gr::tag_t> > d_tags;

      int d_argc;
      char *d_argv;
      QWidget *d_parent;
      TimeDisplayForm *d_main_gui;

      gr::high_res_timer_type d_update_time;
      gr::high_res_timer_type d_last_time;

      // Members used for triggering scope
      trigger_mode d_trigger_mode;
      trigger_slope d_trigger_slope;
      float d_trigger_level;
      int d_trigger_channel;
      int d_trigger_delay;
      pmt::pmt_t d_trigger_tag_key;
      bool d_triggered;
      int d_trigger_count;
      int d_initial_delay; // used for limiting d_trigger_delay

      void _reset();
      void _npoints_resize();
      void _adjust_tags(int adj);
      void _gui_update_trigger();
      void _test_trigger_tags(int nitems);
      void _test_trigger_norm(int nitems, gr_vector_const_void_star inputs);
      bool _test_trigger_slope(const float *in) const;

      // Handles message input port for displaying PDU samples.
      void handle_pdus(pmt::pmt_t msg);

    public:
      time_sink_f_impl(int size, double samp_rate,
		       const std::string &name,
		       int nconnections,
		       QWidget *parent=NULL);
      ~time_sink_f_impl();

      bool check_topology(int ninputs, int noutputs);

      void exec_();
      QWidget*  qwidget();

#ifdef ENABLE_PYTHON
      PyObject* pyqwidget();
#else
      void* pyqwidget();
#endif

      void set_y_axis(double min, double max);
      void set_y_label(const std::string &label,
                       const std::string &unit="");
      void set_update_time(double t);
      void set_title(const std::string &title);
      void set_line_label(int which, const std::string &label);
      void set_line_color(int which, const std::string &color);
      void set_line_width(int which, int width);
      void set_line_style(int which, int style);
      void set_line_marker(int which, int marker);
      void set_nsamps(const int size);
      void set_samp_rate(const double samp_rate);
      void set_line_alpha(int which, double alpha);
      void set_trigger_mode(trigger_mode mode, trigger_slope slope,
                            float level, float delay, int channel,
                            const std::string &tag_key="");

      std::string title();
      std::string line_label(int which);
      std::string line_color(int which);
      int line_width(int which);
      int line_style(int which);
      int line_marker(int which);
      double line_alpha(int which);

      void set_size(int width, int height);

      int nsamps() const;

      void enable_menu(bool en);
      void enable_grid(bool en);
      void enable_autoscale(bool en);
      void enable_stem_plot(bool en);
      void enable_semilogx(bool en);
      void enable_semilogy(bool en);
      void enable_control_panel(bool en);
      void enable_tags(int which, bool en);
      void enable_axis_labels(bool en);
      void disable_legend();

      void reset();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TIME_SINK_F_IMPL_H */
