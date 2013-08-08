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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "const_sink_c_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>
#include <volk/volk.h>
#include <gnuradio/fft/fft.h>
#include <qwt_symbol.h>

namespace gr {
  namespace qtgui {
    
    const_sink_c::sptr
    const_sink_c::make(int size,
		       const std::string &name,
		       int nconnections,
		       QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new const_sink_c_impl(size, name, nconnections, parent));
    }

    const_sink_c_impl::const_sink_c_impl(int size,
					 const std::string &name,
					 int nconnections,
					 QWidget *parent)
      : sync_block("const_sink_c",
		      io_signature::make(nconnections, nconnections, sizeof(gr_complex)),
		      io_signature::make(0, 0, 0)),
	d_size(size), d_name(name),
	d_nconnections(nconnections), d_parent(parent)
    {
      d_main_gui = NULL;

      d_index = 0;

      for(int i = 0; i < d_nconnections; i++) {
	d_residbufs_real.push_back(fft::malloc_double(d_size));
	d_residbufs_imag.push_back(fft::malloc_double(d_size));
	memset(d_residbufs_real[i], 0, d_size*sizeof(double));
	memset(d_residbufs_imag[i], 0, d_size*sizeof(double));
      }

      // Set alignment properties for VOLK
      const int alignment_multiple =
	volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1,alignment_multiple));

      initialize();
    }

    const_sink_c_impl::~const_sink_c_impl()
    {
      if(!d_main_gui->isClosed())
        d_main_gui->close();

      // d_main_gui is a qwidget destroyed with its parent
      for(int i = 0; i < d_nconnections; i++) {
	fft::free(d_residbufs_real[i]);
	fft::free(d_residbufs_imag[i]);
      }
    }

    bool
    const_sink_c_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == d_nconnections;
    }

    void
    const_sink_c_impl::initialize()
    {
      if(qApp != NULL) {
	d_qApplication = qApp;
      }
      else {
	int argc=0;
	char **argv = NULL;
	d_qApplication = new QApplication(argc, argv);
      }

      d_main_gui = new ConstellationDisplayForm(d_nconnections, d_parent);
      d_main_gui->setNPoints(d_size);
      // initialize update time to 10 times a second
      set_update_time(0.1);
    }

    void
    const_sink_c_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    const_sink_c_impl::qwidget()
    {
      return d_main_gui;
    }

    PyObject*
    const_sink_c_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }

    void
    const_sink_c_impl::set_y_axis(double min, double max)
    {
      d_main_gui->setYaxis(min, max);
    }

    void
    const_sink_c_impl::set_x_axis(double min, double max)
    {
      d_main_gui->setXaxis(min, max);
    }

    void
    const_sink_c_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gr::high_res_timer_type tps = gr::high_res_timer_tps();
      d_update_time = t * tps;
      d_main_gui->setUpdateTime(t);
      d_last_time = 0;
    }

    void
    const_sink_c_impl::set_title(const std::string &title)
    {
      d_main_gui->setTitle(title.c_str());
    }

    void
    const_sink_c_impl::set_line_label(int which, const std::string &label)
    {
      d_main_gui->setLineLabel(which, label.c_str());
    }

    void
    const_sink_c_impl::set_line_color(int which, const std::string &color)
    {
      d_main_gui->setLineColor(which, color.c_str());
    }

    void
    const_sink_c_impl::set_line_width(int which, int width)
    {
      d_main_gui->setLineWidth(which, width);
    }

    void
    const_sink_c_impl::set_line_style(int which, int style)
    {
      d_main_gui->setLineStyle(which, (Qt::PenStyle)style);
    }

    void
    const_sink_c_impl::set_line_marker(int which, int marker)
    {
      d_main_gui->setLineMarker(which, (QwtSymbol::Style)marker);
    }

    void
    const_sink_c_impl::set_line_alpha(int which, double alpha)
    {
      d_main_gui->setMarkerAlpha(which, (int)(255.0*alpha));
    }

    void
    const_sink_c_impl::set_size(int width, int height)
    {
      d_main_gui->resize(QSize(width, height));
    }

    std::string
    const_sink_c_impl::title()
    {
      return d_main_gui->title().toStdString();
    }

    std::string
    const_sink_c_impl::line_label(int which)
    {
      return d_main_gui->lineLabel(which).toStdString();
    }

    std::string
    const_sink_c_impl::line_color(int which)
    {
      return d_main_gui->lineColor(which).toStdString();
    }

    int
    const_sink_c_impl::line_width(int which)
    {
      return d_main_gui->lineWidth(which);
    }

    int
    const_sink_c_impl::line_style(int which)
    {
      return d_main_gui->lineStyle(which);
    }

    int
    const_sink_c_impl::line_marker(int which)
    {
      return d_main_gui->lineMarker(which);
    }

    double
    const_sink_c_impl::line_alpha(int which)
    {
      return (double)(d_main_gui->markerAlpha(which))/255.0;
    }

    void
    const_sink_c_impl::set_nsamps(const int newsize)
    {
      gr::thread::scoped_lock lock(d_mutex);

      if(newsize != d_size) {
	// Resize residbuf and replace data
	for(int i = 0; i < d_nconnections; i++) {
	  fft::free(d_residbufs_real[i]);
	  fft::free(d_residbufs_imag[i]);
	  d_residbufs_real[i] = fft::malloc_double(newsize);
	  d_residbufs_imag[i] = fft::malloc_double(newsize);

	  memset(d_residbufs_real[i], 0, newsize*sizeof(double));
	  memset(d_residbufs_imag[i], 0, newsize*sizeof(double));
	}

	// Set new size and reset buffer index 
	// (throws away any currently held data, but who cares?) 
	d_size = newsize;
	d_index = 0;

	d_main_gui->setNPoints(d_size);
      }
    }

    void
    const_sink_c_impl::npoints_resize()
    {
      int newsize = d_main_gui->getNPoints();
      set_nsamps(newsize);
    }

    int
    const_sink_c_impl::nsamps() const
    {
      return d_size;
    }

    void
    const_sink_c_impl::enable_menu(bool en)
    {
      d_main_gui->enableMenu(en);
    }

    void
    const_sink_c_impl::enable_autoscale(bool en)
    {
      d_main_gui->autoScale(en);
    }

    void
    const_sink_c_impl::reset()
    {
      d_index = 0;
    }

    int
    const_sink_c_impl::work(int noutput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items)
    {
      int n=0, j=0, idx=0;
      const gr_complex *in = (const gr_complex*)input_items[idx];

      npoints_resize();

      for(int i=0; i < noutput_items; i+=d_size) {
	unsigned int datasize = noutput_items - i;
	unsigned int resid = d_size-d_index;
	idx = 0;

	// If we have enough input for one full plot, do it
	if(datasize >= resid) {

	  // Fill up residbufs with d_size number of items
	  for(n = 0; n < d_nconnections; n++) {
	    in = (const gr_complex*)input_items[idx++];
	    volk_32fc_deinterleave_64f_x2_u(&d_residbufs_real[n][d_index],
					    &d_residbufs_imag[n][d_index],
					    &in[j], resid);
	  }

	  // Update the plot if its time
	  if(gr::high_res_timer_now() - d_last_time > d_update_time) {
	    d_last_time = gr::high_res_timer_now();
	    d_qApplication->postEvent(d_main_gui,
				      new ConstUpdateEvent(d_residbufs_real,
							   d_residbufs_imag,
							   d_size));
	  }

	  d_index = 0;
	  j += resid;
	}

	// Otherwise, copy what we received into the residbufs for next time
	else {
	  for(n = 0; n < d_nconnections; n++) {
	    in = (const gr_complex*)input_items[idx++];
	    volk_32fc_deinterleave_64f_x2_u(&d_residbufs_real[n][d_index],
					    &d_residbufs_imag[n][d_index],
					    &in[j], datasize);
	  }
	  d_index += datasize;
	  j += datasize;
	}
      }

      return j;
    }

  } /* namespace qtgui */
} /* namespace gr */
