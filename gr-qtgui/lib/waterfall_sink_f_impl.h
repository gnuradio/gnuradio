/* -*- c++ -*- */
/*
 * Copyright 2012,2015,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_WATERFALL_SINK_F_IMPL_H
#define INCLUDED_QTGUI_WATERFALL_SINK_F_IMPL_H

#include <gnuradio/qtgui/waterfall_sink_f.h>

#include <gnuradio/fft/fft.h>
#include <gnuradio/fft/fft_shift.h>
#include <gnuradio/fft/window.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/waterfalldisplayform.h>

namespace gr {
namespace qtgui {

class QTGUI_API waterfall_sink_f_impl : public waterfall_sink_f
{
private:
    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    void initialize();

    int d_fftsize;
    fft::fft_shift<float> d_fft_shift;
    float d_fftavg;
    fft::window::win_type d_wintype;
    std::vector<float> d_window;
    double d_center_freq;
    double d_bandwidth;
    const std::string d_name;
    int d_nconnections;
    int d_nrows;

    const pmt::pmt_t d_port;
    const pmt::pmt_t d_port_bw;

    bool d_shift;
    fft::fft_complex* d_fft;

    int d_index;
    std::vector<float*> d_residbufs;
    std::vector<double*> d_magbufs;
    double* d_pdu_magbuf;
    float* d_fbuf;

    int d_argc;
    char* d_argv;
    QWidget* d_parent;
    WaterfallDisplayForm* d_main_gui;

    gr::high_res_timer_type d_update_time;
    gr::high_res_timer_type d_last_time;

    void windowreset();
    void buildwindow();
    void fftresize();
    void check_clicked();
    void fft(float* data_out, const float* data_in, int size);

    // Handles message input port for setting new bandwidth
    // The message is a PMT pair (intern('bw'), double(bw))
    void handle_set_bw(pmt::pmt_t msg);

    // Handles message input port for setting new center frequency.
    // The message is a PMT pair (intern('freq'), double(frequency)).
    void handle_set_freq(pmt::pmt_t msg);

    // Handles message input port for displaying PDU samples.
    void handle_pdus(pmt::pmt_t msg);

public:
    waterfall_sink_f_impl(int size,
                          int wintype,
                          double fc,
                          double bw,
                          const std::string& name,
                          int nconnections,
                          QWidget* parent = NULL);
    ~waterfall_sink_f_impl();

    bool check_topology(int ninputs, int noutputs);

    void exec_();
    QWidget* qwidget();

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget();
#else
    void* pyqwidget();
#endif

    void clear_data();

    void set_fft_size(const int fftsize);
    int fft_size() const;
    void set_fft_average(const float fftavg);
    float fft_average() const;
    void set_fft_window(const gr::fft::window::win_type win);
    gr::fft::window::win_type fft_window();

    void set_frequency_range(const double centerfreq, const double bandwidth);
    void set_intensity_range(const double min, const double max);

    void set_update_time(double t);
    void set_time_per_fft(double t);
    void set_title(const std::string& title);
    void set_time_title(const std::string& title);
    void set_line_label(unsigned int which, const std::string& label);
    void set_line_alpha(unsigned int which, double alpha);
    void set_color_map(unsigned int which, const int color);
    void set_plot_pos_half(bool half);

    std::string title();
    std::string line_label(unsigned int which);
    double line_alpha(unsigned int which);
    int color_map(unsigned int which);

    void set_size(int width, int height);

    void auto_scale();
    double min_intensity(unsigned int which);
    double max_intensity(unsigned int which);

    void enable_menu(bool en);
    void enable_grid(bool en);
    void disable_legend();
    void enable_axis_labels(bool en);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_WATERFALL_SINK_F_IMPL_H */
