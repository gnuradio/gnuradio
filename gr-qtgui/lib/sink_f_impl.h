/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2011,2012,2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_SINK_F_IMPL_H
#define INCLUDED_QTGUI_SINK_F_IMPL_H

#include <gnuradio/qtgui/sink_f.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/SpectrumGUIClass.h>

namespace gr {
  namespace qtgui {

    class QTGUI_API sink_f_impl : public sink_f
    {
    private:
      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      void initialize();

      int d_fftsize;
      filter::firdes::win_type d_wintype;
      std::vector<float> d_window;
      double d_center_freq;
      double d_bandwidth;
      std::string d_name;

      const pmt::pmt_t d_port;

      bool d_shift;
      fft::fft_complex *d_fft;

      int d_index;
      float *d_residbuf;
      float *d_magbuf;

      bool d_plotfreq, d_plotwaterfall, d_plottime, d_plotconst;

      double d_update_time;

      int d_argc;
      char *d_argv;
      QWidget *d_parent;
      SpectrumGUIClass *d_main_gui;

      void windowreset();
      void buildwindow();
      void fftresize();
      void check_clicked();
      void fft(float *data_out, const float *data_in, int size);

      // Handles message input port for setting new center frequency.
      // The message is a PMT pair (intern('freq'), double(frequency)).
      void handle_set_freq(pmt::pmt_t msg);

    public:
      sink_f_impl(int fftsize, int wintype,
		  double fc, double bw,
		  const std::string &name,
		  bool plotfreq, bool plotwaterfall,
		  bool plottime, bool plotconst,
		  QWidget *parent);
      ~sink_f_impl();

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

      void set_frequency_range(const double centerfreq,
			       const double bandwidth);
      void set_fft_power_db(double min, double max);
      void enable_rf_freq(bool en);

      //void set_time_domain_axis(double min, double max);
      //void set_constellation_axis(double xmin, double xmax,
      //                            double ymin, double ymax);
      //void set_constellation_pen_size(int size);

      void set_update_time(double t);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_SINK_F_IMPL_H */
