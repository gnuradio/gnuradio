/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_VECTOR_SINK_F_IMPL_H
#define INCLUDED_QTGUI_VECTOR_SINK_F_IMPL_H

#include <gnuradio/qtgui/vector_sink_f.h>

#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/vectordisplayform.h>

namespace gr {
namespace qtgui {

class QTGUI_API vector_sink_f_impl : public vector_sink_f
{
private:
    void initialize(const std::string& name,
                    const std::string& x_axis_label,
                    const std::string& y_axis_label,
                    double x_start,
                    double x_step);

    const unsigned int d_vlen; //!< Vector length at input
    float d_vecavg;

    const std::string d_name; //!< Initial title of the plot
    int d_nconnections;       //!< Number of connected streaming ports on input

    const pmt::pmt_t d_port;
    const pmt::pmt_t d_msg; //< Key of outgoing messages

    std::vector<volk::vector<double>> d_magbufs;

    // Required now for Qt; argc must be greater than 0 and argv
    // must have at least one valid character. Must be valid through
    // life of the qApplication:
    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
    char d_zero = 0;
    int d_argc = 1;
    char* d_argv = &d_zero;
    QWidget* d_parent;
    VectorDisplayForm* d_main_gui = nullptr;

    gr::high_res_timer_type d_update_time;
    gr::high_res_timer_type d_last_time;

    // TODO remove this?
    void check_clicked();

    // Handles message input port for setting new center frequency.
    // The message is a PMT pair (intern('freq'), double(frequency)).
    void handle_set_freq(pmt::pmt_t msg);

public:
    vector_sink_f_impl(unsigned int vlen,
                       double x_start,
                       double x_step,
                       const std::string& x_axis_label,
                       const std::string& y_axis_label,
                       const std::string& name,
                       int nconnections,
                       QWidget* parent = NULL);
    ~vector_sink_f_impl() override;

    // Disallow copy/move because raw pointers.
    vector_sink_f_impl(const vector_sink_f_impl&) = delete;
    vector_sink_f_impl(vector_sink_f_impl&&) = delete;
    vector_sink_f_impl& operator=(const vector_sink_f_impl&) = delete;
    vector_sink_f_impl& operator=(vector_sink_f_impl&&) = delete;


    bool check_topology(int ninputs, int noutputs) override;

    void exec_() override;
    QWidget* qwidget() override;

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget() override;
#else
    void* pyqwidget();
#endif

    unsigned int vlen() const override;
    void set_vec_average(const float avg) override;
    float vec_average() const override;

    void set_frequency_range(const double centerfreq, const double bandwidth);
    void set_x_axis(const double start, const double step) override;
    void set_y_axis(double min, double max) override;
    void set_ref_level(double ref_level) override;

    void set_x_axis_label(const std::string& label) override;
    void set_y_axis_label(const std::string& label) override;

    void set_x_axis_units(const std::string& units) override;
    void set_y_axis_units(const std::string& units) override;

    void set_update_time(double t) override;
    void set_title(const std::string& title) override;
    void set_line_label(unsigned int which, const std::string& label) override;
    void set_line_color(unsigned int which, const std::string& color) override;
    void set_line_width(unsigned int which, int width) override;
    void set_line_style(unsigned int which, int style) override;
    void set_line_marker(unsigned int which, int marker) override;
    void set_line_alpha(unsigned int which, double alpha) override;

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
    void clear_max_hold() override;
    void clear_min_hold() override;
    void reset() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_VECTOR_SINK_F_IMPL_H */
