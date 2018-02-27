/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
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

#include "vector_sink_f_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <string.h>
#include <volk/volk.h>
#include <qwt_symbol.h>

namespace gr {
  namespace qtgui {

    static const std::string MSG_PORT_OUT_XVAL = "xval";

    vector_sink_f::sptr
    vector_sink_f::make(
          int vlen,
          double x_start,
          double x_step,
          const std::string &x_axis_label,
          const std::string &y_axis_label,
          const std::string &name,
          int nconnections,
          QWidget *parent
    ) {
      return gnuradio::get_initial_sptr (
          new vector_sink_f_impl(
            vlen,
            x_start,
            x_step,
            x_axis_label,
            y_axis_label,
            name,
            nconnections,
            parent
          )
      );
  }

    vector_sink_f_impl::vector_sink_f_impl(
          int vlen,
          double x_start,
          double x_step,
          const std::string &x_axis_label,
          const std::string &y_axis_label,
          const std::string &name,
          int nconnections,
          QWidget *parent
    ) : sync_block("vector_sink_f",
          io_signature::make(1, -1, sizeof(float) * vlen),
          io_signature::make(0, 0, 0)),
        d_vlen(vlen),
        d_vecavg(1.0),
        d_name(name),
        d_nconnections(nconnections),
        d_msg(pmt::mp("x")),
        d_parent(parent),
        d_port(pmt::mp(MSG_PORT_OUT_XVAL))
    {
      // Required now for Qt; argc must be greater than 0 and argv
      // must have at least one valid character. Must be valid through
      // life of the qApplication:
      // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
      d_argc = 1;
      d_argv = new char;
      d_argv[0] = '\0';

      // setup output message port to post frequency when display is
      // double-clicked
      message_port_register_out(d_port);

      d_main_gui = NULL;

      for(int i = 0; i < d_nconnections; i++) {
        d_magbufs.push_back((double*)volk_malloc(d_vlen*sizeof(double), volk_get_alignment()));
        memset(d_magbufs[i], 0, d_vlen*sizeof(double));
      }

      initialize(
          name,
          x_axis_label,
          y_axis_label,
          x_start,
          x_step
      );
    }

    vector_sink_f_impl::~vector_sink_f_impl()
    {
      if (!d_main_gui->isClosed()) {
        d_main_gui->close();
      }

      for(int i = 0; i < d_nconnections; i++) {
        volk_free(d_magbufs[i]);
      }

      delete d_argv;
    }

    bool
    vector_sink_f_impl::check_topology(int ninputs, int noutputs)
    {
      return ninputs == d_nconnections;
    }

    void
    vector_sink_f_impl::initialize(
          const std::string &name,
          const std::string &x_axis_label,
          const std::string &y_axis_label,
          double x_start,
          double x_step
    ) {
      if(qApp != NULL) {
        d_qApplication = qApp;
      }
      else {
#if QT_VERSION >= 0x040500
        std::string style = prefs::singleton()->get_string("qtgui", "style", "raster");
        QApplication::setGraphicsSystem(QString(style.c_str()));
#endif
        d_qApplication = new QApplication(d_argc, &d_argv);
      }

      // If a style sheet is set in the prefs file, enable it here.
      check_set_qss(d_qApplication);

      d_main_gui = new VectorDisplayForm(d_nconnections, d_parent);
      d_main_gui->setVecSize(d_vlen);
      set_x_axis(x_start, x_step);

      if(! name.empty())
        set_title(name);
      set_x_axis_label(x_axis_label);
      set_y_axis_label(y_axis_label);

      // initialize update time to 10 times a second
      set_update_time(0.1);
    }

    void
    vector_sink_f_impl::exec_()
    {
      d_qApplication->exec();
    }

    QWidget*
    vector_sink_f_impl::qwidget()
    {
      return d_main_gui;
    }

#ifdef ENABLE_PYTHON
    PyObject*
    vector_sink_f_impl::pyqwidget()
    {
      PyObject *w = PyLong_FromVoidPtr((void*)d_main_gui);
      PyObject *retarg = Py_BuildValue("N", w);
      return retarg;
    }
#else
    void *
    vector_sink_f_impl::pyqwidget()
    {
      return NULL;
    }
#endif

    int
    vector_sink_f_impl::vlen() const
    {
      return d_vlen;
    }

    void
    vector_sink_f_impl::set_vec_average(const float avg)
    {
      if (avg < 0 || avg > 1.0) {
        GR_LOG_ALERT(d_logger, "Invalid average value received in set_vec_average(), must be within [0, 1].");
        return;
      }
      d_main_gui->setVecAverage(avg);
      d_vecavg = avg;
    }

    float
    vector_sink_f_impl::vec_average() const
    {
      return d_vecavg;
    }

    void
    vector_sink_f_impl::set_x_axis(const double start, const double step)
    {
      d_main_gui->setXaxis(start, step);
    }

    void
    vector_sink_f_impl::set_y_axis(double min, double max)
    {
      d_main_gui->setYaxis(min, max);
    }

    void
    vector_sink_f_impl::set_ref_level(double ref_level)
    {
      d_main_gui->setRefLevel(ref_level);
    }

    void
    vector_sink_f_impl::set_x_axis_label(const std::string &label)
    {
      d_main_gui->setXAxisLabel(label.c_str());
    }

    void
    vector_sink_f_impl::set_y_axis_label(const std::string &label)
    {
      d_main_gui->setYAxisLabel(label.c_str());
    }

    void
    vector_sink_f_impl::set_x_axis_units(const std::string &units)
    {
      d_main_gui->getPlot()->setXAxisUnit(units.c_str());
    }

    void
    vector_sink_f_impl::set_y_axis_units(const std::string &units)
    {
      d_main_gui->getPlot()->setYAxisUnit(units.c_str());
    }

    void
    vector_sink_f_impl::set_update_time(double t)
    {
      //convert update time to ticks
      gr::high_res_timer_type tps = gr::high_res_timer_tps();
      d_update_time = t * tps;
      d_main_gui->setUpdateTime(t);
      d_last_time = 0;
    }

    void
    vector_sink_f_impl::set_title(const std::string &title)
    {
      d_main_gui->setTitle(title.c_str());
    }

    void
    vector_sink_f_impl::set_line_label(int which, const std::string &label)
    {
      d_main_gui->setLineLabel(which, label.c_str());
    }

    void
    vector_sink_f_impl::set_line_color(int which, const std::string &color)
    {
      d_main_gui->setLineColor(which, color.c_str());
    }

    void
    vector_sink_f_impl::set_line_width(int which, int width)
    {
      d_main_gui->setLineWidth(which, width);
    }

    void
    vector_sink_f_impl::set_line_style(int which, int style)
    {
      d_main_gui->setLineStyle(which, (Qt::PenStyle)style);
    }

    void
    vector_sink_f_impl::set_line_marker(int which, int marker)
    {
      d_main_gui->setLineMarker(which, (QwtSymbol::Style)marker);
    }

    void
    vector_sink_f_impl::set_line_alpha(int which, double alpha)
    {
      d_main_gui->setMarkerAlpha(which, (int)(255.0*alpha));
    }

    void
    vector_sink_f_impl::set_size(int width, int height)
    {
      d_main_gui->resize(QSize(width, height));
    }

    std::string
    vector_sink_f_impl::title()
    {
      return d_main_gui->title().toStdString();
    }

    std::string
    vector_sink_f_impl::line_label(int which)
    {
      return d_main_gui->lineLabel(which).toStdString();
    }

    std::string
    vector_sink_f_impl::line_color(int which)
    {
      return d_main_gui->lineColor(which).toStdString();
    }

    int
    vector_sink_f_impl::line_width(int which)
    {
      return d_main_gui->lineWidth(which);
    }

    int
    vector_sink_f_impl::line_style(int which)
    {
      return d_main_gui->lineStyle(which);
    }

    int
    vector_sink_f_impl::line_marker(int which)
    {
      return d_main_gui->lineMarker(which);
    }

    double
    vector_sink_f_impl::line_alpha(int which)
    {
      return (double)(d_main_gui->markerAlpha(which))/255.0;
    }

    void
    vector_sink_f_impl::enable_menu(bool en)
    {
      d_main_gui->enableMenu(en);
    }

    void
    vector_sink_f_impl::enable_grid(bool en)
    {
      d_main_gui->setGrid(en);
    }

    void
    vector_sink_f_impl::enable_autoscale(bool en)
    {
      d_main_gui->autoScale(en);
    }

    void
    vector_sink_f_impl::clear_max_hold()
    {
      d_main_gui->clearMaxHold();
    }

    void
    vector_sink_f_impl::clear_min_hold()
    {
      d_main_gui->clearMinHold();
    }

    void
    vector_sink_f_impl::reset()
    {
      // nop
    }

    void
    vector_sink_f_impl::check_clicked()
    {
      if(d_main_gui->checkClicked()) {
        double xval = d_main_gui->getClickedXVal();
        message_port_pub(
                         d_port,
                         pmt::cons(d_msg, pmt::from_double(xval))
        );
      }
    }

    int
    vector_sink_f_impl::work(int noutput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items
    ) {
        const float *in = (const float*) input_items[0];

        // See if we generate a message
        check_clicked();

        for(int i = 0; i < noutput_items; i++) {
          if(gr::high_res_timer_now() - d_last_time > d_update_time) {
            for(int n = 0; n < d_nconnections; n++) {
              in = ((const float*)input_items[n]) + d_vlen;
              for(int x = 0; x < d_vlen; x++) {
                d_magbufs[n][x] = (double)((1.0-d_vecavg)*d_magbufs[n][x] + (d_vecavg)*in[x]);
              }
            }
            d_last_time = gr::high_res_timer_now();
            d_qApplication->postEvent(d_main_gui, new FreqUpdateEvent(d_magbufs, d_vlen));
          }
        }

        return noutput_items;
      }

  } /* namespace qtgui */
} /* namespace gr */
