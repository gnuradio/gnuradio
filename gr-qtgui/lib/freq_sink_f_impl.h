/* -*- c++ -*- */
/*
 * Copyright 2012,2014-2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_FREQ_SINK_F_IMPL_H
#define INCLUDED_QTGUI_FREQ_SINK_F_IMPL_H

#include <gnuradio/qtgui/freq_sink_f.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/freqdisplayform.h>

namespace gr {
  namespace qtgui {

    class QTGUI_API freq_sink_f_impl : public freq_sink_f
    {
    private:
      void initialize();

      int d_fftsize;
      int d_tmpbuflen;
      float d_fftavg;
      filter::firdes::win_type d_wintype;
      std::vector<float> d_window;
      double d_center_freq;
      double d_bandwidth;
      std::string d_name;
      int d_nconnections;

      const pmt::pmt_t d_port;

      bool d_shift;
      fft::fft_complex *d_fft;

      int d_index;
      std::vector<float*> d_residbufs;
      std::vector<double*> d_magbufs;
      double* d_pdu_magbuf;
      float *d_fbuf;
      float *d_tmpbuf;

      int d_argc;
      char *d_argv;
      QWidget *d_parent;
      FreqDisplayForm *d_main_gui;

      gr::high_res_timer_type d_update_time;
      gr::high_res_timer_type d_last_time;

      bool windowreset();
      void buildwindow();
      bool fftresize();
      void check_clicked();
      void fft(float *data_out, const float *data_in, int size);

      // Handles message input port for setting new center frequency.
      // The message is a PMT pair (intern('freq'), double(frequency)).
      void handle_set_freq(pmt::pmt_t msg);

      // Handles message input port for displaying PDU samples.
      void handle_pdus(pmt::pmt_t msg);

      // Members used for triggering scope
      trigger_mode d_trigger_mode;
      float d_trigger_level;
      int d_trigger_channel;
      pmt::pmt_t d_trigger_tag_key;
      bool d_triggered;
      int d_trigger_count;

      void _reset();
      void _gui_update_trigger();
      void _test_trigger_tags(int start, int nitems);
      void _test_trigger_norm(int nitems, std::vector<double*> inputs);

    public:
      freq_sink_f_impl(int size, int wintype,
		       double fc, double bw,
		       const std::string &name,
		       int nconnections,
		       QWidget *parent=NULL);
      ~freq_sink_f_impl();

      bool check_topology(int ninputs, int noutputs);

      void exec_();
      QWidget*  qwidget();

#ifdef ENABLE_PYTHON
      PyObject* pyqwidget();
#else
      void* pyqwidget();
#endif

      void set_fft_size(const int fftsize);
      int fft_size() const;
      void set_fft_average(const float fftavg);
      float fft_average() const;
      void set_fft_window(const filter::firdes::win_type win);
      filter::firdes::win_type fft_window();

      void set_frequency_range(const double centerfreq, const double bandwidth);
      void set_y_axis(double min, double max);

      void set_update_time(double t);
      void set_title(const std::string &title);
      void set_y_label(const std::string &label, const std::string &unit);
      void set_line_label(int which, const std::string &label);
      void set_line_color(int which, const std::string &color);
      void set_line_width(int which, int width);
      void set_line_style(int which, int style);
      void set_line_marker(int which, int marker);
      void set_line_alpha(int which, double alpha);
      void set_plot_pos_half(bool half);
      void set_trigger_mode(trigger_mode mode,
                            float level, int channel,
                            const std::string &tag_key="");

      std::string title();
      std::string line_label(int which);
      std::string line_color(int which);
      int line_width(int which);
      int line_style(int which);
      int line_marker(int which);
      double line_alpha(int which);

      void set_size(int width, int height);

      void enable_menu(bool en);
      void enable_grid(bool en);
      void enable_autoscale(bool en);
      void enable_control_panel(bool en);
      void enable_max_hold(bool en);
      void enable_min_hold(bool en);
      void clear_max_hold();
      void clear_min_hold();
      void disable_legend();
      void reset();
      void enable_axis_labels(bool en);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_FREQ_SINK_F_IMPL_H */
