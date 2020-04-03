/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_NUMBER_SINK_H
#define INCLUDED_QTGUI_NUMBER_SINK_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/qtgui/qtgui_types.h>
#include <gnuradio/qtgui/trigger_mode.h>
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
    typedef std::shared_ptr<number_sink> sptr;

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
                     float average = 0,
                     graph_t graph_type = NUM_GRAPH_HORIZ,
                     int nconnections = 1,
                     QWidget* parent = NULL);

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
    virtual void
    set_color(unsigned int which, const std::string& min, const std::string& max) = 0;
    virtual void set_color(unsigned int which, int min, int max) = 0;
    virtual void set_label(unsigned int which, const std::string& label) = 0;
    virtual void set_min(unsigned int which, float min) = 0;
    virtual void set_max(unsigned int which, float max) = 0;
    virtual void set_title(const std::string& title) = 0;
    virtual void set_unit(unsigned int which, const std::string& unit) = 0;
    virtual void set_factor(unsigned int which, float factor) = 0;

    virtual float average() const = 0;
    virtual graph_t graph_type() const = 0;
    virtual std::string color_min(unsigned int which) const = 0;
    virtual std::string color_max(unsigned int which) const = 0;
    virtual std::string label(unsigned int which) const = 0;
    virtual float min(unsigned int which) const = 0;
    virtual float max(unsigned int which) const = 0;
    virtual std::string title() const = 0;
    virtual std::string unit(unsigned int which) const = 0;
    virtual float factor(unsigned int which) const = 0;

    virtual void enable_menu(bool en = true) = 0;
    virtual void enable_autoscale(bool en = true) = 0;

    virtual void reset() = 0;

    QApplication* d_qApplication;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_NUMBER_SINK_H */
