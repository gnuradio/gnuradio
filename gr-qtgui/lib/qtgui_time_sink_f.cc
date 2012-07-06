/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#include <qtgui_time_sink_f.h>
#include <gr_io_signature.h>
#include <string.h>
#include <volk/volk.h>

#include <QTimer>

qtgui_time_sink_f_sptr
qtgui_make_time_sink_f (int size, double bw,
			const std::string &name,
			int nconnections,
			QWidget *parent)
{
  return gnuradio::get_initial_sptr(new qtgui_time_sink_f (size, bw, name,
							   nconnections, parent));
}

qtgui_time_sink_f::qtgui_time_sink_f (int size, double bw,
				      const std::string &name,
				      int nconnections,
				      QWidget *parent)
  : gr_sync_block ("time_sink_f",
		   gr_make_io_signature (nconnections, nconnections, sizeof(float)),
		   gr_make_io_signature (0, 0, 0)),
    d_size(size), d_bandwidth(bw), d_name(name),
    d_nconnections(nconnections), d_parent(parent)
{
  d_main_gui = NULL;

  d_index = 0;

  for(int i = 0; i < d_nconnections; i++) {
    d_residbufs.push_back(gri_fft_malloc_double(d_size));
  }

  // Set alignment properties for VOLK
  const int alignment_multiple =
    volk_get_alignment() / sizeof(gr_complex);
  set_alignment(std::max(1,alignment_multiple));

  initialize();
}

qtgui_time_sink_f::~qtgui_time_sink_f()
{
  // d_main_gui is a qwidget destroyed with its parent
  for(int i = 0; i < d_nconnections; i++) {
    gri_fft_free(d_residbufs[i]);
  }
}

void
qtgui_time_sink_f::initialize()
{
  if(qApp != NULL) {
    d_qApplication = qApp;
  }
  else {
    int argc=0;
    char **argv = NULL;
    d_qApplication = new QApplication(argc, argv);
  }

  d_main_gui = new TimeDisplayForm(d_nconnections, d_parent);

  // initialize update time to 10 times a second
  set_update_time(0.1);
  d_last_time = 0;
}


void
qtgui_time_sink_f::exec_()
{
  d_qApplication->exec();
}

QWidget*
qtgui_time_sink_f::qwidget()
{
  return d_main_gui;
}

PyObject*
qtgui_time_sink_f::pyqwidget()
{
  PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
  PyObject *retarg = Py_BuildValue("N", w);
  return retarg;
}

void
qtgui_time_sink_f::set_time_domain_axis(double min, double max)
{
  d_main_gui->setTimeDomainAxis(min, max);
}

void
qtgui_time_sink_f::set_update_time(double t)
{
  //convert update time to ticks
  gruel::high_res_timer_type tps = gruel::high_res_timer_tps();
  d_update_time = t * tps;
  d_main_gui->setUpdateTime(t);
}

void
qtgui_time_sink_f::set_title(int which, const std::string &title)
{
  d_main_gui->setTitle(which, title.c_str());
}

void
qtgui_time_sink_f::set_color(int which, const std::string &color)
{
  d_main_gui->setColor(which, color.c_str());
}

int
qtgui_time_sink_f::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  int n=0, j=0, idx=0;
  const float *in = (const float*)input_items[idx];

  for(int i=0; i < noutput_items; i+=d_size) {
    unsigned int datasize = noutput_items - i;
    unsigned int resid = d_size-d_index;
    idx = 0;

    // If we have enough input for one full plot, do it
    if(datasize >= resid) {

      // Fill up residbufs with d_size number of items
      for(n = 0; n < d_nconnections; n++) {
	in = (const float*)input_items[idx++];
	if(is_unaligned()) {
	  volk_32f_convert_64f_u(&d_residbufs[n][d_index],
				 &in[j], resid);
	}
	else {
	  volk_32f_convert_64f_a(&d_residbufs[n][d_index],
				 &in[j], resid);
	}
      }

      // Update the plot if its time
      if(gruel::high_res_timer_now() - d_last_time > d_update_time) {
	d_last_time = gruel::high_res_timer_now();
	d_qApplication->postEvent(d_main_gui,
				  new TimeUpdateEvent(d_residbufs, d_size));
      }

      d_index = 0;
      j += resid;
    }
    // Otherwise, copy what we received into the residbufs for next time
    // because we set the output_multiple, this should never need to be called
    else {
      for(n = 0; n < d_nconnections; n++) {
	in = (const float*)input_items[idx++];
	if(is_unaligned()) {
	  volk_32f_convert_64f_u(&d_residbufs[n][d_index],
				 &in[j], datasize);
	}
	else {
	  volk_32f_convert_64f_a(&d_residbufs[n][d_index],
				 &in[j], datasize);
	}
      }
      d_index += datasize;
      j += datasize;
    }
  }

  return noutput_items;
}
