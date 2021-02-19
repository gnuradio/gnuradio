/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_NUMBER_SINK_IMPL_H
#define INCLUDED_QTGUI_NUMBER_SINK_IMPL_H

#include <gnuradio/qtgui/number_sink.h>

#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/numberdisplayform.h>

namespace gr {
namespace qtgui {

class QTGUI_API number_sink_impl : public number_sink
{
private:
    void initialize();

    size_t d_itemsize;
    float d_average;
    graph_t d_type;
    int d_nconnections;

    int d_index, d_start, d_end;
    std::vector<volk::vector<double>> d_buffers;
    std::vector<std::vector<gr::tag_t>> d_tags;

    // Required now for Qt; argc must be greater than 0 and argv
    // must have at least one valid character. Must be valid through
    // life of the qApplication:
    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
    char d_zero = 0;
    int d_argc = 1;
    char* d_argv = &d_zero;
    QWidget* d_parent;
    NumberDisplayForm* d_main_gui = nullptr;

    std::vector<float> d_avg_value;
    std::vector<filter::single_pole_iir<float, float, float>> d_iir;

    gr::high_res_timer_type d_update_time;
    gr::high_res_timer_type d_last_time;

    void _reset();
    void _npoints_resize();
    void _gui_update_trigger();

    float get_item(const void* input_items, int n);

public:
    number_sink_impl(size_t itemsize,
                     float average = 0,
                     graph_t graph_type = NUM_GRAPH_HORIZ,
                     int nconnections = 1,
                     QWidget* parent = NULL);
    ~number_sink_impl() override;

    bool check_topology(int ninputs, int noutputs) override;

    void exec_() override;
    QWidget* qwidget() override;

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget() override;
#else
    void* pyqwidget();
#endif

    void set_update_time(double t) override;
    void set_average(const float avg) override;
    void set_graph_type(const graph_t type) override;
    void set_color(unsigned int which,
                   const std::string& min,
                   const std::string& max) override;
    void set_color(unsigned int which, int min, int max) override;
    void set_label(unsigned int which, const std::string& label) override;
    void set_min(unsigned int which, float min) override;
    void set_max(unsigned int which, float max) override;
    void set_title(const std::string& title) override;
    void set_unit(unsigned int which, const std::string& unit) override;
    void set_factor(unsigned int which, float factor) override;

    float average() const override;
    graph_t graph_type() const override;
    std::string color_min(unsigned int which) const override;
    std::string color_max(unsigned int which) const override;
    std::string label(unsigned int which) const override;
    float min(unsigned int which) const override;
    float max(unsigned int which) const override;
    std::string title() const override;
    std::string unit(unsigned int which) const override;
    float factor(unsigned int which) const override;

    void enable_menu(bool en) override;
    void enable_autoscale(bool en = true) override;

    void reset() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_NUMBER_SINK_IMPL_H */
