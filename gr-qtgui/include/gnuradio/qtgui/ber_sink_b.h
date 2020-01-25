/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_BER_SINK_B_H
#define INCLUDED_QTGUI_BER_SINK_B_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/block.h>
#include <gnuradio/qtgui/api.h>
#include <qapplication.h>


namespace gr {
namespace qtgui {

class QTGUI_API ber_sink_b : virtual public block
{
public:
    // gr::fec::ber_sink_b::sptr
    typedef boost::shared_ptr<ber_sink_b> sptr;

    static sptr make(std::vector<float> esnos,
                     int curves = 1,
                     int berminerrors = 100,
                     float berLimit = -7.0,
                     std::vector<std::string> curvenames = std::vector<std::string>(),
                     QWidget* parent = NULL);

    virtual void exec_() = 0;

#ifdef ENABLE_PYTHON
    virtual PyObject* pyqwidget() = 0;
#else
    virtual void* pyqwidget() = 0;
#endif

    virtual void set_y_axis(double min, double max) = 0;
    virtual void set_x_axis(double min, double max) = 0;

    virtual void set_update_time(double t) = 0;
    virtual void set_title(const std::string& title) = 0;
    virtual void set_line_label(unsigned int which, const std::string& label) = 0;
    virtual void set_line_color(unsigned int which, const std::string& color) = 0;
    virtual void set_line_width(unsigned int which, int width) = 0;
    virtual void set_line_style(unsigned int which, int style) = 0;
    virtual void set_line_marker(unsigned int which, int marker) = 0;

    virtual void set_line_alpha(unsigned int which, double alpha) = 0;

    virtual std::string title() = 0;
    virtual std::string line_label(unsigned int which) = 0;
    virtual std::string line_color(unsigned int which) = 0;
    virtual int line_width(unsigned int which) = 0;
    virtual int line_style(unsigned int which) = 0;
    virtual int line_marker(unsigned int which) = 0;
    virtual double line_alpha(unsigned int which) = 0;

    virtual void set_size(int width, int height) = 0;

    virtual void enable_menu(bool en = true) = 0;
    virtual void enable_autoscale(bool en) = 0;
    virtual int nsamps() const = 0;

    QApplication* d_qApplication;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_BER_SINK_B_H */
