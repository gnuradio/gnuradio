/* -*- c++ -*- */
/*
 * Copyright 2012,2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_TIME_RASTER_SINK_F_IMPL_H
#define INCLUDED_QTGUI_TIME_RASTER_SINK_F_IMPL_H

#include <gnuradio/qtgui/time_raster_sink_f.h>

#include <gnuradio/fft/fft.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/timerasterdisplayform.h>

namespace gr {
namespace qtgui {

class QTGUI_API time_raster_sink_f_impl : public time_raster_sink_f
{
private:
    void initialize();

    const std::string d_name;
    int d_nconnections;

    int d_index;
    std::vector<double*> d_residbufs;

    float* d_tmpflt;

    int d_argc;
    char* d_argv;
    QWidget* d_parent;
    TimeRasterDisplayForm* d_main_gui;

    int d_icols;
    double d_rows, d_cols;
    std::vector<float> d_mult;
    std::vector<float> d_offset;
    double d_samp_rate;

    gr::high_res_timer_type d_update_time;
    gr::high_res_timer_type d_last_time;

    void _ncols_resize();

    // Handles message input port for displaying PDU samples.
    void handle_pdus(pmt::pmt_t msg);

public:
    time_raster_sink_f_impl(double samp_rate,
                            double rows,
                            double cols,
                            const std::vector<float>& mult,
                            const std::vector<float>& offset,
                            const std::string& name,
                            int nconnections,
                            QWidget* parent = NULL);
    ~time_raster_sink_f_impl();

    bool check_topology(int ninputs, int noutputs);

    void exec_();
    QWidget* qwidget();

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget();
#else
    void* pyqwidget();
#endif

    void set_update_time(double t);
    void set_title(const std::string& title);
    void set_line_label(unsigned int which, const std::string& label);
    void set_line_color(unsigned int which, const std::string& color);
    void set_line_width(unsigned int which, int width);
    void set_line_style(unsigned int which, Qt::PenStyle style);
    void set_line_marker(unsigned int which, QwtSymbol::Style marker);
    void set_line_alpha(unsigned int which, double alpha);
    void set_color_map(unsigned int which, const int color);

    std::string title();
    std::string line_label(unsigned int which);
    std::string line_color(unsigned int which);
    int line_width(unsigned int which);
    int line_style(unsigned int which);
    int line_marker(unsigned int which);
    double line_alpha(unsigned int which);
    int color_map(unsigned int which);

    void set_size(int width, int height);

    void set_samp_rate(const double samp_rate);
    void set_num_rows(double rows);
    void set_num_cols(double cols);

    double num_rows();
    double num_cols();

    void set_multiplier(const std::vector<float>& mult);
    void set_offset(const std::vector<float>& offset);

    void set_intensity_range(float min, float max);

    void enable_menu(bool en);
    void enable_grid(bool en);
    void enable_autoscale(bool en);
    void enable_axis_labels(bool en);
    void reset();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TIME_RASTER_SINK_F_IMPL_H */
