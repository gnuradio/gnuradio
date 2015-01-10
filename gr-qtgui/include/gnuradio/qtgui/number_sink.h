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

#ifndef INCLUDED_QTGUI_NUMBER_SINK_H
#define INCLUDED_QTGUI_NUMBER_SINK_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/qtgui/trigger_mode.h>
#include <gnuradio/qtgui/qtgui_types.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>

namespace gr {
  namespace qtgui {

    /*!
     * \brief A graphical sink to display numerical values of input streams.
     * \ingroup instrumentation_blk
     * \ingroup qtgui_blk
     *
     * \details
     *
     * Displays the data stream in as a number in a simple text box
     * GUI along with an optional bar graph. The bar graph can be set
     * to horizontal (NUM_GRAPH_HORIZ), vertical (NUM_GRAPH_VERT), or
     * no graph (NUM_GRAPH_NONE).
     *
     * The displayed value can be the average of the input stream, in
     * which case all items received are averaged. If not averaging,
     * the display simply samples a value in the data stream based on
     * the update time of this block.
     *
     * Note that due to a flaw in the implementation, this block
     * cannot receive integer value inputs. It will take chars,
     * shorts, and floats and properly convert them by setting
     * itemsize of the constructor to one of these three values
     * (sizeof_char, sizeof_short, and sizeof_float, respectively). If
     * using integers, the block treats these as floats. Instead, put
     * the integer input stream through an gr::blocks::int_to_float
     * converter block.
     */
    class QTGUI_API number_sink : virtual public sync_block
    {
    public:

      // gr::qtgui::number_sink::sptr
      typedef boost::shared_ptr<number_sink> sptr;

      /*!
       * \brief Build a number sink
       *
       * \param itemsize Size of input item stream
       * \param average Averaging coefficient (0 - 1)
       * \param graph_type Type of graph to use (number_sink::graph_t)
       * \param nconnections number of signals connected to sink
       * \param parent a QWidget parent object, if any
       */
      static sptr make(size_t itemsize,
                       float average=0,
                       graph_t graph_type=NUM_GRAPH_HORIZ,
		       int nconnections=1,
		       QWidget *parent=NULL);

      virtual void exec_() = 0;
      virtual QWidget* qwidget() = 0;

#ifdef ENABLE_PYTHON
      virtual PyObject* pyqwidget() = 0;
#else
      virtual void* pyqwidget() = 0;
#endif

      virtual void set_update_time(double t) = 0;
      virtual void set_average(const float avg) = 0;
      virtual void set_graph_type(const graph_t type) = 0;
      virtual void set_color(int which,
                             const std::string &min,
                             const std::string &max) = 0;
      virtual void set_color(int which, int min, int max) = 0;
      virtual void set_label(int which, const std::string &label) = 0;
      virtual void set_min(int which, float min) = 0;
      virtual void set_max(int which, float max) = 0;
      virtual void set_title(const std::string &title) = 0;
      virtual void set_unit(int which, const std::string &unit) = 0;
      virtual void set_factor(int which, float factor) = 0;

      virtual float average() const = 0;
      virtual graph_t graph_type() const = 0;
      virtual std::string color_min(int which) const = 0;
      virtual std::string color_max(int which) const = 0;
      virtual std::string label(int which) const = 0;
      virtual float min(int which) const = 0;
      virtual float max(int which) const = 0;
      virtual std::string title() const = 0;
      virtual std::string unit(int which) const = 0;
      virtual float factor(int which) const = 0;

      virtual void enable_menu(bool en=true) = 0;
      virtual void enable_autoscale(bool en=true) = 0;

      virtual void reset() = 0;

      QApplication *d_qApplication;
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_NUMBER_SINK_H */
