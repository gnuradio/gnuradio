/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include "time_raster_sink_c_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>
#include <volk/volk.h>

namespace gr {
  namespace qtgui {
    
    time_raster_sink_c::sptr
    time_raster_sink_c::make(unsignedint rows,
			     unsigned int cols,
			     const std::string &name,
			     QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new time_raster_sink_c_impl(rows, cols,
				     name, parent));
    }

    time_raster_sink_c_impl::time_raster_sink_c_impl(unsignedint rows,
						     unsigned int cols,
						     const std::string &name,
						     QWidget *parent)
      : sync_block("time_raster_sink_c",
		      io_signature::make(1, -1, sizeof(gr_complex)),
		      io_signature::make(0, 0, 0)),
	d_name(name), d_nconnections(1), d_parent(parent),
	d_rows(rows), d_cols(cols)
    {
      d_main_gui = NULL;

      d_index = 0;
      for(int i = 0; i < d_nconnections; i++) {
	d_residbufs.push_back(fft::malloc_complex(rows));
	memset(d_residbufs[i], 0, rows*sizeof(float));
      }

      initialize();
    }

    time_raster_sink_c_impl::~time_raster_sink_c_impl()
    {
      if(!d_main_gui->isClosed())
        d_main_gui->close();

      for(int i = 0; i < d_nconnections; i++) {
	fft::free(d_residbufs[i]);
      }
    }

    void
    time_raster_sink_c_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned int ninputs = ninput_items_required.size();
      for (unsigned int i = 0; i < ninputs; i++) {
	ninput_items_required[i] = std::min(d_rows, 8191);
      }
    }

    void
    time_raster_sink_c_impl::initialize()
    {
      if(qApp != NULL) {
	d_qApplication = qApp;
      }
      else {
	int argc = 0;
	char **argv = NULL;
	d_qApplication = new QApplication(argc, argv);
      }

      d_main_gui = new TimeRasterDisplayForm(d_nconnections, d_rows, d_cols, d_parent);

      // initialize update time to 10 times a second
      set_update_time(0.1);
      d_last_time = 0;
    }

    void
    time_raster_sink_c_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    time_raster_sink_c_impl::qwidget()
    {
      return d_main_gui;
    }

    PyObject*
    time_raster_sink_c_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }

    void
    time_raster_sink_c_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gr::high_res_timer_type tps = gr::high_res_timer_tps();
      d_update_time = t * tps;
      d_main_gui->setUpdateTime(t);
    }

    void
    time_raster_sink_c_impl::set_title(const std::string &title)
    {
      d_main_gui->setTitle(title.c_str());
    }

    void
    time_raster_sink_c_impl::set_line_label(const std::string &label)
    {
      d_main_gui->setLineLabel(0, label.c_str());
    }

    void
    time_raster_sink_c_impl::set_line_color(const std::string &color)
    {
      d_main_gui->setLineColor(0, color.c_str());
    }

    void
    time_raster_sink_c_impl::set_line_width(int width)
    {
      d_main_gui->setLineWidth(0, width);
    }

    void
    time_raster_sink_c_impl::set_line_style(Qt::PenStyle style)
    {
      d_main_gui->setLineStyle(0, style);
    }

    void
    time_raster_sink_c_impl::set_line_marker(QwtSymbol::Style marker)
    {
      d_main_gui->setLineMarker(0, marker);
    }

    void
    time_raster_sink_c_impl::set_size(int width, int height)
    {
      d_main_gui->resize(QSize(width, height));
    }

    int
    time_raster_sink_c_impl::work(int noutput_items,
				gr_vector_const_void_star &input_items,
				gr_vector_void_star &output_items)
    {
      int j=0;
      const gr_complex *in = (const gr_complex*)input_items[0];

      for(int i=0; i < noutput_items; i+=d_cols) {
	unsigned int datasize = noutput_items - i;
	unsigned int resid = d_cols-d_index;

	// If we have enough input for one full column
	if(datasize >= resid) {

	  for(int n = 0; n < d_nconnections; n++) {
	    in = (const gr_complex*)input_items[n];
	    memcpy(d_residbufs[n]+d_index, &in[j], sizeof(gr_complex)*resid);
	  }
      
	  if(gr::high_res_timer_now() - d_last_time > d_update_time) {
	    d_last_time = gr::high_res_timer_now();
	    d_qApplication->postEvent(d_main_gui,
				      new TimeRasterUpdateEvent(d_residbufs,
								d_fftsize,
								d_last_time));
	  }

	  d_index = 0;
	  j += resid;
	}
	// Otherwise, copy what we received into the residbuf for next time
	else {
	  for(int n = 0; n < d_nconnections; n++) {
	    in = (const gr_complex*)input_items[n];
	    memcpy(d_residbufs[n]+d_index, &in[j], sizeof(gr_complex)*datasize);
	  }
	  d_index += datasize;
	  j += datasize;
	}
      }

      return j;
    }

  } /* namespace qtgui */
} /* namespace gr */
