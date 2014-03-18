/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include "number_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <string.h>
#include <volk/volk.h>
#include <gnuradio/fft/fft.h>
#include <qwt_symbol.h>

namespace gr {
  namespace qtgui {

    number_sink::sptr
    number_sink::make(size_t itemsize,
                      float average,
                      graph_t graph_type,
                      int nconnections,
                      QWidget *parent)
    {
      return gnuradio::get_initial_sptr
	(new number_sink_impl(itemsize, average,
                              graph_type, nconnections, parent));
    }

    number_sink_impl::number_sink_impl(size_t itemsize,
                                       float average,
                                       graph_t graph_type,
                                       int nconnections,
                                       QWidget *parent)
      : sync_block("number_sink",
                   io_signature::make(nconnections, nconnections, itemsize),
                   io_signature::make(0, 0, 0)),
	d_itemsize(itemsize), d_average(average),
	d_type(graph_type), d_nconnections(nconnections), d_parent(parent),
        d_avg_value(nconnections), d_iir(nconnections)
    {
      for(int n = 0; n < d_nconnections; n++) {
        d_avg_value[n] = 0;
        d_iir[n].set_taps(d_average);
      }

      // Required now for Qt; argc must be greater than 0 and argv
      // must have at least one valid character. Must be valid through
      // life of the qApplication:
      // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
      d_argc = 1;
      d_argv = new char;
      d_argv[0] = '\0';

      d_main_gui = NULL;

      // Set alignment properties for VOLK
      const int alignment_multiple =
	volk_get_alignment() / d_itemsize;
      set_alignment(std::max(1,alignment_multiple));

      initialize();
    }

    number_sink_impl::~number_sink_impl()
    {
      //if(!d_main_gui->isClosed())
      //  d_main_gui->close();

      delete d_argv;
    }

    bool
    number_sink_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == d_nconnections;
    }

    void
    number_sink_impl::initialize()
    {
      if(qApp != NULL) {
	d_qApplication = qApp;
      }
      else {
	d_qApplication = new QApplication(d_argc, &d_argv);
      }

      d_main_gui = new NumberDisplayForm(d_nconnections, d_type, d_parent);

      // initialize update time to 10 times a second
      set_update_time(0.1);
    }

    void
    number_sink_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    number_sink_impl::qwidget()
    {
      return d_main_gui;
    }

#ifdef ENABLE_PYTHON
    PyObject*
    number_sink_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }
#else
    void *
    number_sink_impl::pyqwidget()
    {
      return NULL;
    }
#endif

    void
    number_sink_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gr::high_res_timer_type tps = gr::high_res_timer_tps();
      d_main_gui->setUpdateTime(t);
      d_update_time = t * tps;
      d_last_time = 0;
    }

    void
    number_sink_impl::set_average(const float avg)
    {
      d_average = avg;
      for(int n = 0; n < d_nconnections; n++) {
        d_avg_value[n] = 0;
        d_iir[n].set_taps(d_average);
      }
    }

    void
    number_sink_impl::set_graph_type(const graph_t type)
    {
      d_main_gui->setGraphType(type);
    }

    void
    number_sink_impl::set_color(int which,
                                const std::string &min,
                                const std::string &max)
    {
      d_main_gui->setColor(which,
                           QColor(min.c_str()),
                           QColor(max.c_str()));
    }

    void
    number_sink_impl::set_color(int which, int min, int max)
    {
      d_main_gui->setColor(which, QColor(min), QColor(max));
    }

    void
    number_sink_impl::set_label(int which, const std::string &label)
    {
      d_main_gui->setLabel(which, label);
    }

    void
    number_sink_impl::set_min(int which, float min)
    {
      d_main_gui->setScaleMin(which, min);
    }

    void
    number_sink_impl::set_max(int which, float max)
    {
      return d_main_gui->setScaleMax(which, max);
    }

    float
    number_sink_impl::average() const
    {
      return d_average;
    }

    graph_t
    number_sink_impl::graph_type() const
    {
      return d_main_gui->graphType();
    }

    std::string
    number_sink_impl::color_min(int which) const
    {
      return d_main_gui->colorMin(which).name().toStdString();
    }

    std::string
    number_sink_impl::color_max(int which) const
    {
      return d_main_gui->colorMax(which).name().toStdString();
    }

    std::string
    number_sink_impl::label(int which) const
    {
      return d_main_gui->label(which);
    }

    float
    number_sink_impl::min(int which) const
    {
      return d_main_gui->scaleMin(which);
    }

    float
    number_sink_impl::max(int which) const
    {
      return d_main_gui->scaleMax(which);
    }

    void
    number_sink_impl::enable_menu(bool en)
    {
      //d_main_gui->enableMenu(en);
    }

    void
    number_sink_impl::enable_autoscale(bool en)
    {
      d_main_gui->autoScale(en);
    }

    void
    number_sink_impl::reset()
    {
      gr::thread::scoped_lock lock(d_mutex);
      _reset();
    }

    void
    number_sink_impl::_reset()
    {
    }

    int
    number_sink_impl::work(int noutput_items,
			   gr_vector_const_void_star &input_items,
			   gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock lock(d_mutex);

      float new_avg = d_main_gui->average();
      set_update_time(d_main_gui->updateTime());
      if(new_avg != d_average) {
        set_average(new_avg);
      }

      if(d_average > 0) {
        for(int n = 0; n < d_nconnections; n++) {
          float *in = (float*)input_items[n];
          for(int i = 0; i < noutput_items; i++) {
            d_avg_value[n] = d_iir[n].filter(in[i]);
          }
        }
      }

      // Plot if we are able to update
      if(gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();
        std::vector<float> d(d_nconnections);
        if(d_average > 0) {
          for(int n = 0; n < d_nconnections; n++)
            d[n] = d_avg_value[n];
        }
        else {
          for(int n = 0; n < d_nconnections; n++)
            d[n] = ((float*)input_items[n])[0];
        }
        d_qApplication->postEvent(d_main_gui,
                                  new NumberUpdateEvent(d));
      }

      return noutput_items;;
    }

  } /* namespace qtgui */
} /* namespace gr */
