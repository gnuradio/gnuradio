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
    ~histogram_sink_f_impl() override;

    bool check_topology(int ninputs, int noutputs) override;

    void exec_() override;
    QWidget* qwidget() override;

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget() override;
#else
    void* pyqwidget();
#endif

    void set_y_axis(double min, double max) override;
    void set_x_axis(double min, double max) override;
    void set_update_time(double t) override;
    void set_title(const std::string& title) override;
    void set_line_label(unsigned int which, const std::string& label) override;
    void set_line_color(unsigned int which, const std::string& color) override;
    void set_line_width(unsigned int which, int width) override;
    void set_line_style(unsigned int which, int style) override;
    void set_line_marker(unsigned int which, int marker) override;
    void set_line_alpha(unsigned int which, double alpha) override;
    void set_nsamps(const int newsize) override;
    void set_bins(const int bins) override;
    void enable_axis_labels(bool en) override;

    std::string title() override;
    std::string line_label(unsigned int which) override;
    std::string line_color(unsigned int which) override;
    int line_width(unsigned int which) override;
    int line_style(unsigned int which) override;
    int line_marker(unsigned int which) override;
    double line_alpha(unsigned int which) override;

    void set_size(int width, int height) override;

    void enable_menu(bool en) override;
    void enable_grid(bool en) override;
    void enable_autoscale(bool en) override;
    void enable_semilogx(bool en) override;
    void enable_semilogy(bool en) override;
    void enable_accumulate(bool en) override;
    void disable_legend() override;
    void autoscalex() override;
    int nsamps() const override;
    int bins() const override;
    void reset() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_HISTOGRAM_SINK_F_IMPL_H */
