/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_EYE_SINK_F_IMPL_H
#define INCLUDED_QTGUI_EYE_SINK_F_IMPL_H

#include <gnuradio/qtgui/eye_sink_f.h>

#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/eyedisplayform.h>

namespace gr {
namespace qtgui {

class QTGUI_API eye_sink_f_impl : public eye_sink_f
{
private:
    void initialize();

    int d_size, d_buffer_size;
    double d_samp_rate;
    unsigned int d_nconnections;

    int d_index, d_start, d_end;
    std::vector<float*> d_fbuffers;
    std::vector<double*> d_buffers;
    std::vector<std::vector<gr::tag_t>> d_tags;

    int d_argc;
    char* d_argv;
    QWidget* d_parent;
    EyeDisplayForm* d_main_gui;
    gr::high_res_timer_type d_update_time;
    gr::high_res_timer_type d_last_time;

    // Members used for triggering scope
    gr::qtgui::trigger_mode d_trigger_mode;
    gr::qtgui::trigger_slope d_trigger_slope;
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
    bool _test_trigger_slope(const float* in) const;

    // Handles message input port for displaying PDU samples.
    void handle_pdus(pmt::pmt_t msg);

public:
    eye_sink_f_impl(int size,
                    double samp_rate,
                    unsigned int nconnections,
                    QWidget* parent = NULL);
    ~eye_sink_f_impl();

    bool check_topology(int ninputs, int noutputs);

    void exec_();
    QWidget* qwidget();

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget();
#else
    void* pyqwidget();
#endif

    void set_y_axis(double min, double max);
    void set_y_label(const std::string& label, const std::string& unit = "");
    void set_update_time(double t);
    void set_samp_per_symbol(unsigned int sps);
    void set_line_label(unsigned int which, const std::string& label);
    void set_line_color(unsigned int which, const std::string& color);
    void set_line_width(unsigned int which, int width);
    void set_line_style(unsigned int which, int style);
    void set_line_marker(unsigned int which, int marker);
    void set_nsamps(const int size);
    void set_samp_rate(const double samp_rate);
    void set_line_alpha(unsigned int which, double alpha);
    void set_trigger_mode(gr::qtgui::trigger_mode mode,
                          gr::qtgui::trigger_slope slope,
                          float level,
                          float delay,
                          int channel,
                          const std::string& tag_key = "");

    std::string title();
    std::string line_label(unsigned int which);
    std::string line_color(unsigned int which);
    int line_width(unsigned int which);
    int line_style(unsigned int which);
    int line_marker(unsigned int which);
    double line_alpha(unsigned int which);

    void set_size(int width, int height);

    int nsamps() const;

    void enable_menu(bool en);
    void enable_grid(bool en);
    void enable_autoscale(bool en);
    void enable_stem_plot(bool en); // Used by parent class, do not remove
    void enable_semilogx(bool en);  // Used by parent class, do not remove
    void enable_semilogy(bool en);  // Used by parent class, do not remove
    void enable_control_panel(bool en);
    void enable_tags(unsigned int which, bool en);
    void enable_tags(bool en);
    void enable_axis_labels(bool en);
    void disable_legend();

    void reset();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_EYE_SINK_F_IMPL_H */
