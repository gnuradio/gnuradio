/* -*- c++ -*- */
/*
 * Copyright 2008,2009,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_QTGUI_SINK_F_H
#define INCLUDED_QTGUI_SINK_F_H

#include <Python.h>
#include <gr_qtgui_api.h>
#include <gr_block.h>
#include <gr_firdes.h>
#include <gri_fft.h>
#include <qapplication.h>
#include "SpectrumGUIClass.h"

class qtgui_sink_f;
typedef boost::shared_ptr<qtgui_sink_f> qtgui_sink_f_sptr;

GR_QTGUI_API qtgui_sink_f_sptr qtgui_make_sink_f (int fftsize, int wintype,
				     double fc=0, double bw=1.0,
				     const std::string &name="Spectrum Display",
				     bool plotfreq=true, bool plotwaterfall=true,
				     bool plottime=true, bool plotconst=false,
				     QWidget *parent=NULL);

/*!
 * \brief A graphical sink to display freq, spec, and time.
 * \ingroup qtgui_blk
 *
 * This is a QT-based graphical sink the takes a float stream and
 * plots it. The default action is to plot the signal as a PSD (FFT),
 * spectrogram (waterfall), and time domain plots. The plots may be
 * turned off by setting the appropriate boolean value in the
 * constructor to False.
 */

class GR_QTGUI_API qtgui_sink_f : public gr_block
{
private:
  friend GR_QTGUI_API qtgui_sink_f_sptr qtgui_make_sink_f (int fftsize, int wintype,
					      double fc, double bw,
					      const std::string &name,
					      bool plotfreq, bool plotwaterfall,
					      bool plottime, bool plotconst,
					      QWidget *parent);
  qtgui_sink_f (int fftsize, int wintype,
		double fc, double bw,
		const std::string &name,
		bool plotfreq, bool plotwaterfall,
		bool plottime, bool plotconst,
		QWidget *parent);

  void forecast(int noutput_items, gr_vector_int &ninput_items_required);

  void initialize();

  int d_fftsize;
  gr_firdes::win_type d_wintype;
  std::vector<float> d_window;
  double d_center_freq;
  double d_bandwidth;
  std::string d_name;
  
  bool d_shift;
  gri_fft_complex *d_fft;

  int d_index;
  float *d_residbuf;

  bool d_plotfreq, d_plotwaterfall, d_plottime, d_plotconst;

  double d_update_time;

  QWidget *d_parent;
  SpectrumGUIClass *d_main_gui; 

  void windowreset();
  void buildwindow();
  void fftresize();
  void fft(const float *data_in, int size);
  
public:
  ~qtgui_sink_f();
  void exec_();
  QWidget*  qwidget();
  PyObject* pyqwidget();

  void set_frequency_range(const double centerfreq,
			   const double bandwidth);

  void set_time_domain_axis(double min, double max);
  void set_constellation_axis(double xmin, double xmax,
			      double ymin, double ymax);
  void set_constellation_pen_size(int size);
  void set_frequency_axis(double min, double max);

  void set_update_time(double t);

  QApplication *d_qApplication;

  int general_work (int noutput_items,
		    gr_vector_int &ninput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_QTGUI_SINK_F_H */
