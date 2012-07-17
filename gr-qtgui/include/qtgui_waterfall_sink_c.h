/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_WATERFALL_SINK_C_H
#define INCLUDED_QTGUI_WATERFALL_SINK_C_H

#include <Python.h>
#include <gr_qtgui_api.h>
#include <gr_sync_block.h>
#include <gr_firdes.h>
#include <fft/fft.h>
#include <qapplication.h>
#include <waterfalldisplayform.h>

class qtgui_waterfall_sink_c;
typedef boost::shared_ptr<qtgui_waterfall_sink_c> qtgui_waterfall_sink_c_sptr;

GR_QTGUI_API qtgui_waterfall_sink_c_sptr
qtgui_make_waterfall_sink_c(int fftsize, int wintype,
			    double fc, double bw,
			    const std::string &name,
			    int nconnections=1,
			    QWidget *parent=NULL);

/*!
 * \brief A graphical sink to display multiple signals in time.
 * \ingroup qtgui_blk
 *
 * This is a QT-based graphical sink the takes set of a complex
 * streams and plots them in the time domain. For each signal, both
 * the signal's I and Q parts are plotted, and they are all plotted
 * with a different color, and the \a set_title and \a set_color
 * functions can be used to change the lable and color for a given
 * input number.
 */
class GR_QTGUI_API qtgui_waterfall_sink_c : public gr_sync_block
{
private:
  friend GR_QTGUI_API qtgui_waterfall_sink_c_sptr
    qtgui_make_waterfall_sink_c(int fftsize, int wintype,
				double fc, double bw,
				const std::string &name,
				int nconnections,
				QWidget *parent);

  qtgui_waterfall_sink_c(int size, int wintype,
			 double fc, double bw,
			 const std::string &name,
			 int nconnections,
			 QWidget *parent=NULL);

  void forecast(int noutput_items, gr_vector_int &ninput_items_required);

  void initialize();

  int d_fftsize;
  gr_firdes::win_type d_wintype;
  std::vector<float> d_window;
  double d_center_freq;
  double d_bandwidth;
  std::string d_name;
  int d_nconnections;

  bool d_shift;
  gr::fft::fft_complex *d_fft;

  int d_index;
  std::vector<gr_complex*> d_residbufs;
  std::vector<double*> d_magbufs;

  QWidget *d_parent;
  WaterfallDisplayForm *d_main_gui;

  gruel::high_res_timer_type d_update_time;
  gruel::high_res_timer_type d_last_time;

  void windowreset();
  void buildwindow();
  void fftresize();
  void fft(float *data_out, const gr_complex *data_in, int size);

public:
  ~qtgui_waterfall_sink_c();
  void exec_();
  QWidget*  qwidget();
  PyObject* pyqwidget();

  void set_frequency_range(const double centerfreq, const double bandwidth);

  void set_update_time(double t);
  void set_title(int which, const std::string &title);
  void set_color(int which, const std::string &color);

  void set_resize(int width, int height);

  QApplication *d_qApplication;

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_QTGUI_WATERFALL_SINK_C_H */
