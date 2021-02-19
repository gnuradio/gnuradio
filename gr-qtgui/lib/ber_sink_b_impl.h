/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_BER_SINK_B_IMPL_H
#define INCLUDED_QTGUI_BER_SINK_B_IMPL_H

#include <gnuradio/qtgui/ber_sink_b.h>

#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/constellationdisplayform.h>

#include <volk/volk_alloc.hh>

namespace gr {
namespace qtgui {

class QTGUI_API ber_sink_b_impl : public ber_sink_b
{
private:
    void initialize();

    std::vector<volk::vector<double>> d_esno_buffers;
    std::vector<volk::vector<double>> d_ber_buffers;

    ConstellationDisplayForm* d_main_gui = nullptr;
    gr::high_res_timer_type d_update_time;
    std::vector<int> d_total_errors;
    int d_ber_min_errors;
    float d_ber_limit;
    QWidget* d_parent;
    int d_nconnections;
    int d_curves;
    gr::high_res_timer_type d_last_time;
    std::vector<int> d_total;

    // int compBER(unsigned char *inBuffer1, unsigned char *inBuffer2,int buffSize);

public:
    ber_sink_b_impl(std::vector<float> esnos,
                    int curves = 1,
                    int ber_min_errors = 100,
                    float ber_limit = -7.0,
                    std::vector<std::string> curvenames = std::vector<std::string>(),
                    QWidget* parent = NULL);
    ~ber_sink_b_impl() override;

    // Disallow copy/move because of the raw pointers.
    ber_sink_b_impl(const ber_sink_b_impl&) = delete;
    ber_sink_b_impl& operator=(const ber_sink_b_impl&) = delete;
    ber_sink_b_impl(ber_sink_b_impl&&) = delete;
    ber_sink_b_impl& operator=(ber_sink_b_impl&&) = delete;

    bool check_topology(int ninputs, int noutputs) override;

    void exec_() override;
    QWidget* qwidget();

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
    void enable_autoscale(bool en) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /*INCLUDED_QTGUI_BER_SINK_B_IMPL_H*/
