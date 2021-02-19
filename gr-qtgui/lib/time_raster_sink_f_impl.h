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

    int d_index = 0;
    std::vector<volk::vector<double>> d_residbufs;

    double d_rows, d_cols;
    int d_icols;
    volk::vector<float> d_tmpflt;

    // Required now for Qt; argc must be greater than 0 and argv
    // must have at least one valid character. Must be valid through
    // life of the qApplication:
    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
    char d_zero = 0;
    int d_argc = 1;
    char* d_argv = &d_zero;
    QWidget* d_parent;
    TimeRasterDisplayForm* d_main_gui = nullptr;

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
    ~time_raster_sink_f_impl() override;

    bool check_topology(int ninputs, int noutputs) override;

    void exec_() override;
    QWidget* qwidget() override;

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget() override;
#else
    void* pyqwidget();
#endif

    void set_x_label(const std::string& label) override;
    void set_x_range(double start, double end) override;
    void set_y_label(const std::string& label) override;
    void set_y_range(double start, double end) override;
    void set_update_time(double t) override;
    void set_title(const std::string& title) override;
    void set_line_label(unsigned int which, const std::string& label) override;
    void set_line_color(unsigned int which, const std::string& color) override;
    void set_line_width(unsigned int which, int width) override;
    void set_line_style(unsigned int which, Qt::PenStyle style) override;
    void set_line_marker(unsigned int which, QwtSymbol::Style marker) override;
    void set_line_alpha(unsigned int which, double alpha) override;
    void set_color_map(unsigned int which, const int color) override;

    std::string title() override;
    std::string line_label(unsigned int which) override;
    std::string line_color(unsigned int which) override;
    int line_width(unsigned int which) override;
    int line_style(unsigned int which) override;
    int line_marker(unsigned int which) override;
    double line_alpha(unsigned int which) override;
    int color_map(unsigned int which) override;

    void set_size(int width, int height) override;

    void set_samp_rate(const double samp_rate) override;
    void set_num_rows(double rows) override;
    void set_num_cols(double cols) override;

    double num_rows() override;
    double num_cols() override;

    void set_multiplier(const std::vector<float>& mult) override;
    void set_offset(const std::vector<float>& offset) override;

    void set_intensity_range(float min, float max) override;

    void enable_menu(bool en) override;
    void enable_grid(bool en) override;
    void enable_autoscale(bool en) override;
    void enable_axis_labels(bool en) override;
    void reset() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TIME_RASTER_SINK_F_IMPL_H */
