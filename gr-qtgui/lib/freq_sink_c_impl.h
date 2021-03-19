/* -*- c++ -*- */
/*
 * Copyright 2012,2015,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_FREQ_SINK_C_IMPL_H
#define INCLUDED_QTGUI_FREQ_SINK_C_IMPL_H

#include <gnuradio/qtgui/freq_sink_c.h>

#include <gnuradio/fft/fft.h>
#include <gnuradio/fft/fft_shift.h>
#include <gnuradio/fft/window.h>
#include <gnuradio/high_res_timer.h>
#include <gnuradio/qtgui/freqdisplayform.h>

namespace gr {
namespace qtgui {

class QTGUI_API freq_sink_c_impl : public freq_sink_c
{
private:
    void initialize();

    int d_fftsize;
    fft::fft_shift<float> d_fft_shift;
    float d_fftavg;
    fft::window::win_type d_wintype;
    std::vector<float> d_window;
    bool d_window_normalize = false; //<! If true, window functions will be normalized
    double d_center_freq;
    double d_bandwidth;
    const std::string d_name;
    int d_nconnections;

    const pmt::pmt_t d_port;
    const pmt::pmt_t d_port_bw;

    // Perform fftshift operation;
    // this is usually desired when plotting
    std::unique_ptr<fft::fft_complex_fwd> d_fft;

    int d_index = 0;
    std::vector<volk::vector<gr_complex>> d_residbufs;
    std::vector<volk::vector<double>> d_magbufs;
    double* d_pdu_magbuf;
    volk::vector<float> d_fbuf;

    // Required now for Qt; argc must be greater than 0 and argv
    // must have at least one valid character. Must be valid through
    // life of the qApplication:
    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
    char d_zero = 0;
    int d_argc = 1;
    char* d_argv = &d_zero;
    QWidget* d_parent;
    FreqDisplayForm* d_main_gui = nullptr;

    gr::high_res_timer_type d_update_time;
    gr::high_res_timer_type d_last_time;

    bool windowreset();
    void buildwindow();
    bool fftresize();
    void check_clicked();
    void fft(float* data_out, const gr_complex* data_in, int size);

    // Handles message input port for setting new bandwidth
    // The message is a PMT pair (intern('bw'), double(bw))
    void handle_set_bw(pmt::pmt_t msg);

    // Handles message input port for setting new center frequency.
    // The message is a PMT pair (intern('freq'), double(frequency)).
    void handle_set_freq(pmt::pmt_t msg);

    // Handles message input port for displaying PDU samples.
    void handle_pdus(pmt::pmt_t msg);

    // Members used for triggering scope
    trigger_mode d_trigger_mode;
    float d_trigger_level;
    int d_trigger_channel;
    pmt::pmt_t d_trigger_tag_key;
    bool d_triggered;
    int d_trigger_count;

    void _reset();
    void _gui_update_trigger();
    void _test_trigger_tags(int start, int nitems);
    void _test_trigger_norm(int nitems, std::vector<volk::vector<double>> inputs);

public:
    freq_sink_c_impl(int size,
                     int wintype,
                     double fc,
                     double bw,
                     const std::string& name,
                     int nconnections,
                     QWidget* parent = NULL);
    ~freq_sink_c_impl() override;

    // Disallow copy/move because of the raw pointers.
    freq_sink_c_impl(const freq_sink_c_impl&) = delete;
    freq_sink_c_impl& operator=(const freq_sink_c_impl&) = delete;
    freq_sink_c_impl(freq_sink_c_impl&&) = delete;
    freq_sink_c_impl& operator=(freq_sink_c_impl&&) = delete;

    bool check_topology(int ninputs, int noutputs) override;

    void exec_() override;
    QWidget* qwidget() override;

#ifdef ENABLE_PYTHON
    PyObject* pyqwidget() override;
#else
    void* pyqwidget();
#endif

    void set_fft_size(const int fftsize) override;
    int fft_size() const override;
    void set_fft_average(const float fftavg) override;
    float fft_average() const override;
    void set_fft_window(const fft::window::win_type win) override;
    fft::window::win_type fft_window() override;
    void set_fft_window_normalized(const bool enable) override;

    void set_frequency_range(const double centerfreq, const double bandwidth) override;
    void set_y_axis(double min, double max) override;

    void set_update_time(double t) override;

    void set_title(const std::string& title) override;
    void set_y_label(const std::string& label, const std::string& unit) override;
    void set_line_label(unsigned int which, const std::string& label) override;
    void set_line_color(unsigned int which, const std::string& color) override;
    void set_line_width(unsigned int which, int width) override;
    void set_line_style(unsigned int which, int style) override;
    void set_line_marker(unsigned int which, int marker) override;
    void set_line_alpha(unsigned int which, double alpha) override;
    void set_trigger_mode(trigger_mode mode,
                          float level,
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

    void enable_menu(bool en) override;
    void enable_grid(bool en) override;
    void enable_autoscale(bool en) override;
    void enable_control_panel(bool en) override;
    void enable_max_hold(bool en) override;
    void enable_min_hold(bool en) override;
    void clear_max_hold() override;
    void clear_min_hold() override;
    void disable_legend() override;
    void reset() override;
    void enable_axis_labels(bool en) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_FREQ_SINK_C_IMPL_H */
