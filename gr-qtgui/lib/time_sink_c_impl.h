/* -*- c++ -*- */
/*
 * Copyright 2011-2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_TIME_SINK_C_IMPL_H
#define INCLUDED_QTGUI_TIME_SINK_C_IMPL_H

#include <gnuradio/qtgui/time_sink_c.h>

#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/timedisplayform.h>

namespace gr {
namespace qtgui {

class QTGUI_API time_sink_c_impl : public time_sink_c
{
private:
    void initialize();

    int d_size, d_buffer_size;
    double d_samp_rate;
    const std::string d_name;
    unsigned int d_nconnections;

    const pmt::pmt_t d_tag_key;

    int d_index, d_start, d_end;
    std::vector<volk::vector<gr_complex>> d_cbuffers;
    std::vector<volk::vector<double>> d_buffers;
    std::vector<std::vector<gr::tag_t>> d_tags;

    // Required now for Qt; argc must be greater than 0 and argv
    // must have at least one valid character. Must be valid through
    // life of the qApplication:
    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
    char d_zero;
    int d_argc = 1;
    char* d_argv = &d_zero;
    QWidget* d_parent;
    TimeDisplayForm* d_main_gui = nullptr;

    gr::high_res_timer_type d_update_time;
    gr::high_res_timer_type d_last_time;

    // Members used for triggering scope
    trigger_mode d_trigger_mode;
    trigger_slope d_trigger_slope;
    float d_trigger_level;
    int d_trigger_channel;
    int d_trigger_delay;
    pmt::pmt_t d_trigger_tag_key;
    bool d_triggered;
    int d_trigger_count;

    void _reset();
    void _npoints_resize();
    void _adjust_tags(int adj);
    void _gui_update_trigger();
    void _test_trigger_tags(int nitems);
    void _test_trigger_norm(int nitems, gr_vector_const_void_star inputs);
    bool _test_trigger_slope(const gr_complex* in) const;

    // Handles message input port for displaying PDU samples.
    void handle_pdus(pmt::pmt_t msg);

public:
    time_sink_c_impl(int size,
                     double samp_rate,
                     const std::string& name,
                     unsigned int nconnections,
                     QWidget* parent = NULL);
    ~time_sink_c_impl() override;

    bool check_topology(int ninputs, int noutputs) override;

    void exec_() override;
    QWidget* qwidget() override;

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget() override;
#else
    void* pyqwidget();
#endif

    void set_y_axis(double min, double max) override;
    void set_y_label(const std::string& label, const std::string& unit = "") override;
    void set_update_time(double t) override;
    void set_title(const std::string& title) override;
    void set_line_label(unsigned int which, const std::string& label) override;
    void set_line_color(unsigned int which, const std::string& color) override;
    void set_line_width(unsigned int which, int width) override;
    void set_line_style(unsigned int which, int style) override;
    void set_line_marker(unsigned int which, int marker) override;
    void set_nsamps(const int size) override;
    void set_samp_rate(const double samp_rate) override;
    void set_line_alpha(unsigned int which, double alpha) override;
    void set_trigger_mode(trigger_mode mode,
                          trigger_slope slope,
                          float level,
                          float delay,
                          int channel,
                          const std::string& tag_key = "") override;

    std::string title() override;
    std::string line_label(unsigned int which) override;
    std::string line_color(unsigned int which) override;
    int line_width(unsigned int which) override;
    int line_style(unsigned int which) override;
    int line_marker(unsigned int which) override;
    double line_alpha(unsigned int which) override;

    void set_size(int width, int height) override;

    int nsamps() const override;

    void enable_menu(bool en) override;
    void enable_grid(bool en) override;
    void enable_autoscale(bool en) override;
    void enable_stem_plot(bool en) override;
    void enable_semilogx(bool en) override;
    void enable_semilogy(bool en) override;
    void enable_control_panel(bool en) override;
    void enable_tags(unsigned int which, bool en) override;
    void enable_tags(bool en) override;
    void enable_axis_labels(bool en) override;
    void disable_legend() override;

    void reset() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TIME_SINK_C_IMPL_H */
