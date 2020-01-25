/* -*- c++ -*- */
/*
 * Copyright 2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_HISTOGRAM_SINK_F_IMPL_H
#define INCLUDED_QTGUI_HISTOGRAM_SINK_F_IMPL_H

#include <gnuradio/qtgui/histogram_sink_f.h>

#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/histogramdisplayform.h>

namespace gr {
namespace qtgui {

class QTGUI_API histogram_sink_f_impl : public histogram_sink_f
{
private:
    void initialize();

    int d_size;
    int d_bins;
    double d_xmin, d_xmax;
    const std::string d_name;
    int d_nconnections;

    int d_index;
    std::vector<double*> d_residbufs;

    int d_argc;
    char* d_argv;
    QWidget* d_parent;
    HistogramDisplayForm* d_main_gui;

    gr::high_res_timer_type d_update_time;
    gr::high_res_timer_type d_last_time;

    void npoints_resize();

    // Handles message input port for displaying PDU samples.
    void handle_pdus(pmt::pmt_t msg);

public:
    histogram_sink_f_impl(int size,
                          int bins,
                          double xmin,
                          double xmax,
                          const std::string& name,
                          int nconnections,
                          QWidget* parent = NULL);
    ~histogram_sink_f_impl();

    bool check_topology(int ninputs, int noutputs);

    void exec_();
    QWidget* qwidget();

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget();
#else
    void* pyqwidget();
#endif

    void set_y_axis(double min, double max);
    void set_x_axis(double min, double max);
    void set_update_time(double t);
    void set_title(const std::string& title);
    void set_line_label(unsigned int which, const std::string& label);
    void set_line_color(unsigned int which, const std::string& color);
    void set_line_width(unsigned int which, int width);
    void set_line_style(unsigned int which, int style);
    void set_line_marker(unsigned int which, int marker);
    void set_line_alpha(unsigned int which, double alpha);
    void set_nsamps(const int newsize);
    void set_bins(const int bins);
    void enable_axis_labels(bool en);

    std::string title();
    std::string line_label(unsigned int which);
    std::string line_color(unsigned int which);
    int line_width(unsigned int which);
    int line_style(unsigned int which);
    int line_marker(unsigned int which);
    double line_alpha(unsigned int which);

    void set_size(int width, int height);

    void enable_menu(bool en);
    void enable_grid(bool en);
    void enable_autoscale(bool en);
    void enable_semilogx(bool en);
    void enable_semilogy(bool en);
    void enable_accumulate(bool en);
    void disable_legend();
    void autoscalex();
    int nsamps() const;
    int bins() const;
    void reset();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_HISTOGRAM_SINK_F_IMPL_H */
