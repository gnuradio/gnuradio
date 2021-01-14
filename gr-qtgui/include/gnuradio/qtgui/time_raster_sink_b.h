/* -*- c++ -*- */
/*
 * Copyright 2012,2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_TIME_RASTER_SINK_B_H
#define INCLUDED_QTGUI_TIME_RASTER_SINK_B_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/qtgui/api.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>
#include <qwt_symbol.h>

namespace gr {
namespace qtgui {

/*!
 * \brief A graphical sink to display multiple signals on a
 * time_raster plot.
 * \ingroup instrumentation_blk
 * \ingroup qtgui_blk
 *
 * \details
 * This is a QT-based graphical sink that takes in byte
 * streams and plots a time_raster (spectrogram) plot.
 *
 * Input stream: This expects a bit stream (0, 1 in the LSB of a
 * byte). It will display packed bytes but the display will have
 * to be autoscaled.
 *
 * The sink supports plotting streaming byte/char data or
 * messages. The message port is named "in". The two modes cannot
 * be used simultaneously, and \p nconnections should be set to 0
 * when using the message mode. GRC handles this issue by
 * providing the "Float Message" type that removes the streaming
 * port(s).
 *
 * This sink can plot messages that contain either uniform vectors
 * of byte/char values (pmt::is_{u,s}32vector) or PDUs where the
 * data is a uniform vector of byte/char values.
 */
class QTGUI_API time_raster_sink_b : virtual public sync_block
{
public:
    // gr::qtgui::time_raster_sink_b::sptr
    typedef std::shared_ptr<time_raster_sink_b> sptr;

    /*!
     * \brief Build a bit time raster sink.
     *
     * \param samp_rate sample rate of signal
     * \param cols number of cols to plot
     * \param rows number of rows to plot
     * \param mult vector of floats as a scaling multiplier for each input stream
     * \param offset vector of floats as an offset for each input stream
     * \param name title for the plot
     * \param nconnections number of streams connected
     * \param parent a QWidget parent object, if any
     */
    static sptr make(double samp_rate,
                     double rows,
                     double cols,
                     const std::vector<float>& mult,
                     const std::vector<float>& offset,
                     const std::string& name,
                     int nconnections = 1,
                     QWidget* parent = NULL);

    virtual void exec_() = 0;
    virtual QWidget* qwidget() = 0;

#ifdef ENABLE_PYTHON
    virtual PyObject* pyqwidget() = 0;
#else
    virtual void* pyqwidget() = 0;
#endif

    virtual void set_x_label(const std::string& label) = 0;
    virtual void set_x_range(double start, double end) = 0;
    virtual void set_y_label(const std::string& label) = 0;
    virtual void set_y_range(double start, double end) = 0;
    virtual void set_update_time(double t) = 0;
    virtual void set_title(const std::string& title) = 0;
    virtual void set_line_label(unsigned int which, const std::string& label) = 0;
    virtual void set_line_color(unsigned int which, const std::string& color) = 0;
    virtual void set_line_width(unsigned int which, int width) = 0;
    virtual void set_line_style(unsigned int which, Qt::PenStyle style) = 0;
    virtual void set_line_marker(unsigned int which, QwtSymbol::Style marker) = 0;
    virtual void set_line_alpha(unsigned int which, double alpha) = 0;
    virtual void set_color_map(unsigned int which, const int color) = 0;

    virtual std::string title() = 0;
    virtual std::string line_label(unsigned int which) = 0;
    virtual std::string line_color(unsigned int which) = 0;
    virtual int line_width(unsigned int which) = 0;
    virtual int line_style(unsigned int which) = 0;
    virtual int line_marker(unsigned int which) = 0;
    virtual double line_alpha(unsigned int which) = 0;
    virtual int color_map(unsigned int which) = 0;

    virtual void set_size(int width, int height) = 0;

    virtual void set_samp_rate(const double samp_rate) = 0;
    virtual void set_num_rows(double rows) = 0;
    virtual void set_num_cols(double cols) = 0;

    virtual double num_rows() = 0;
    virtual double num_cols() = 0;

    virtual void set_multiplier(const std::vector<float>& mult) = 0;
    virtual void set_offset(const std::vector<float>& offset) = 0;

    virtual void set_intensity_range(float min, float max) = 0;

    virtual void enable_menu(bool en) = 0;
    virtual void enable_grid(bool en) = 0;
    virtual void enable_autoscale(bool en) = 0;
    virtual void enable_axis_labels(bool en = true) = 0;
    virtual void reset() = 0;

    QApplication* d_qApplication;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TIME_RASTER_SINK_B_H */
