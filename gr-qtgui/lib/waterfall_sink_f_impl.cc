/* -*- c++ -*- */
/*
 * Copyright 2012,2014-2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "waterfall_sink_f_impl.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>

#include <volk/volk.h>

#include <cstring>

namespace gr {
namespace qtgui {

waterfall_sink_f::sptr waterfall_sink_f::make(int fftsize,
                                              int wintype,
                                              double fc,
                                              double bw,
                                              const std::string& name,
                                              int nconnections,
                                              QWidget* parent)
{
    return gnuradio::make_block_sptr<waterfall_sink_f_impl>(
        fftsize, wintype, fc, bw, name, nconnections, parent);
}

waterfall_sink_f_impl::waterfall_sink_f_impl(int fftsize,
                                             int wintype,
                                             double fc,
                                             double bw,
                                             const std::string& name,
                                             int nconnections,
                                             QWidget* parent)
    : sync_block("waterfall_sink_f",
                 io_signature::make(0, nconnections, sizeof(float)),
                 io_signature::make(0, 0, 0)),
      d_fftsize(fftsize),
      d_fft_shift(fftsize),
      d_fftavg(1.0),
      d_wintype((fft::window::win_type)(wintype)),
      d_center_freq(fc),
      d_bandwidth(bw),
      d_name(name),
      d_nconnections(nconnections),
      d_nrows(200),
      d_port(pmt::mp("freq")),
      d_port_bw(pmt::mp("bw")),
      d_fft(std::make_unique<fft::fft_complex_fwd>(d_fftsize)),
      d_residbufs(d_nconnections + 1), // One extra "connection" for the PDU memory.
      d_magbufs(d_nconnections + 1),   // One extra "connection" for the PDU memory.
      d_fbuf(fftsize),
      d_parent(parent)
{
    resize_bufs(d_fftsize);
    buildwindow();

    initialize();

    // setup bw input port
    message_port_register_in(d_port_bw);
    set_msg_handler(d_port_bw, [this](pmt::pmt_t msg) { this->handle_set_bw(msg); });

    // setup output message port to post frequency when display is
    // double-clicked
    message_port_register_out(d_port);
    message_port_register_in(d_port);
    set_msg_handler(d_port, [this](pmt::pmt_t msg) { this->handle_set_freq(msg); });

    // setup PDU handling input port
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->handle_pdus(msg); });
}

waterfall_sink_f_impl::~waterfall_sink_f_impl()
{
    if (!d_main_gui->isClosed())
        d_main_gui->close();
}

bool waterfall_sink_f_impl::check_topology(int ninputs, int noutputs)
{
    return ninputs == d_nconnections;
}

void waterfall_sink_f_impl::forecast(int noutput_items,
                                     gr_vector_int& ninput_items_required)
{
    unsigned int ninputs = ninput_items_required.size();
    for (unsigned int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = std::min(d_fftsize, 8191);
    }
}

void waterfall_sink_f_impl::initialize()
{
    if (qApp != NULL) {
        d_qApplication = qApp;
    } else {
        d_qApplication = new QApplication(d_argc, &d_argv);
    }

    // If a style sheet is set in the prefs file, enable it here.
    check_set_qss(d_qApplication);

    int numplots = (d_nconnections > 0) ? d_nconnections : 1;
    d_main_gui = new WaterfallDisplayForm(numplots, d_parent);
    set_fft_window(d_wintype);
    set_fft_size(d_fftsize);
    set_frequency_range(d_center_freq, d_bandwidth);

    if (!d_name.empty())
        set_title(d_name);

    // initialize update time to 10 times a second
    set_update_time(0.1);
}

void waterfall_sink_f_impl::exec_() { d_qApplication->exec(); }

QWidget* waterfall_sink_f_impl::qwidget() { return d_main_gui; }

void waterfall_sink_f_impl::clear_data() { d_main_gui->clearData(); }

void waterfall_sink_f_impl::set_fft_size(const int fftsize)
{
    d_main_gui->setFFTSize(fftsize);
}

int waterfall_sink_f_impl::fft_size() const { return d_fftsize; }

void waterfall_sink_f_impl::set_fft_average(const float fftavg)
{
    d_main_gui->setFFTAverage(fftavg);
}

float waterfall_sink_f_impl::fft_average() const { return d_fftavg; }

void waterfall_sink_f_impl::set_fft_window(const fft::window::win_type win)
{
    d_main_gui->setFFTWindowType(win);
}

fft::window::win_type waterfall_sink_f_impl::fft_window() { return d_wintype; }

void waterfall_sink_f_impl::set_frequency_range(const double centerfreq,
                                                const double bandwidth)
{
    d_center_freq = centerfreq;
    d_bandwidth = bandwidth;
    d_main_gui->setFrequencyRange(d_center_freq, d_bandwidth);
}

void waterfall_sink_f_impl::set_intensity_range(const double min, const double max)
{
    d_main_gui->setIntensityRange(min, max);
}

void waterfall_sink_f_impl::set_update_time(double t)
{
    // convert update time to ticks
    gr::high_res_timer_type tps = gr::high_res_timer_tps();
    d_update_time = t * tps;
    d_main_gui->setUpdateTime(t);
    d_last_time = 0;
}

void waterfall_sink_f_impl::set_title(const std::string& title)
{
    d_main_gui->setTitle(title.c_str());
}

void waterfall_sink_f_impl::set_time_title(const std::string& title)
{
    d_main_gui->setTimeTitle(title);
}

void waterfall_sink_f_impl::set_line_label(unsigned int which, const std::string& label)
{
    d_main_gui->setLineLabel(which, label.c_str());
}

void waterfall_sink_f_impl::set_color_map(unsigned int which, const int color)
{
    d_main_gui->setColorMap(which, color);
}

void waterfall_sink_f_impl::set_line_alpha(unsigned int which, double alpha)
{
    d_main_gui->setAlpha(which, (int)(255.0 * alpha));
}

void waterfall_sink_f_impl::set_size(int width, int height)
{
    d_main_gui->resize(QSize(width, height));
}

void waterfall_sink_f_impl::set_plot_pos_half(bool half)
{
    d_main_gui->setPlotPosHalf(half);
}

std::string waterfall_sink_f_impl::title() { return d_main_gui->title().toStdString(); }

std::string waterfall_sink_f_impl::line_label(unsigned int which)
{
    return d_main_gui->lineLabel(which).toStdString();
}

int waterfall_sink_f_impl::color_map(unsigned int which)
{
    return d_main_gui->getColorMap(which);
}

double waterfall_sink_f_impl::line_alpha(unsigned int which)
{
    return (double)(d_main_gui->getAlpha(which)) / 255.0;
}

void waterfall_sink_f_impl::auto_scale() { d_main_gui->autoScale(); }

double waterfall_sink_f_impl::min_intensity(unsigned int which)
{
    return d_main_gui->getMinIntensity(which);
}

double waterfall_sink_f_impl::max_intensity(unsigned int which)
{
    return d_main_gui->getMaxIntensity(which);
}

void waterfall_sink_f_impl::enable_menu(bool en) { d_main_gui->enableMenu(en); }

void waterfall_sink_f_impl::enable_grid(bool en) { d_main_gui->setGrid(en); }

void waterfall_sink_f_impl::enable_axis_labels(bool en) { d_main_gui->setAxisLabels(en); }

void waterfall_sink_f_impl::disable_legend() { d_main_gui->disableLegend(); }

void waterfall_sink_f_impl::fft(float* data_out, const float* data_in, int size)
{
    // float to complex conversion
    gr_complex* dst = d_fft->get_inbuf();
    for (int i = 0; i < size; i++)
        dst[i] = data_in[i];

    if (!d_window.empty()) {
        volk_32fc_32f_multiply_32fc(d_fft->get_inbuf(), dst, &d_window.front(), size);
    }

    d_fft->execute(); // compute the fft

    volk_32fc_s32f_x2_power_spectral_density_32f(
        data_out, d_fft->get_outbuf(), size, 1.0, size);

    d_fft_shift.shift(data_out, size);
}

void waterfall_sink_f_impl::windowreset()
{
    gr::thread::scoped_lock lock(d_setlock);

    fft::window::win_type newwintype;
    newwintype = d_main_gui->getFFTWindowType();
    if (d_wintype != newwintype) {
        d_wintype = newwintype;
        buildwindow();
    }
}

void waterfall_sink_f_impl::buildwindow()
{
    d_window.clear();
    if (d_wintype != fft::window::WIN_NONE) {
        d_window = fft::window::build(d_wintype, d_fftsize);
    }
}

void waterfall_sink_f_impl::resize_bufs(int size)
{
    // Resize residbuf and replace data.
    for (auto& buf : d_residbufs) {
        buf.clear();
        buf.resize(size);
    }
    for (auto& mag : d_magbufs) {
        mag.clear();
        mag.resize(size);
    }

    // Expand PDU buffer to required size.
    auto& last_magbuf = d_magbufs[d_magbufs.size() - 1];
    last_magbuf.resize(size * d_nrows);
    d_pdu_magbuf = last_magbuf.data();
}

void waterfall_sink_f_impl::fftresize()
{
    gr::thread::scoped_lock lock(d_setlock);

    int newfftsize = d_main_gui->getFFTSize();
    d_fftavg = d_main_gui->getFFTAverage();

    if (newfftsize != d_fftsize) {
        resize_bufs(newfftsize);

        // Set new fft size and reset buffer index
        // (throws away any currently held data, but who cares?)
        d_fftsize = newfftsize;
        d_index = 0;

        // Reset window to reflect new size
        buildwindow();

        // Reset FFTW plan for new size
        d_fft = std::make_unique<fft::fft_complex_fwd>(d_fftsize);

        d_fft_shift.resize(d_fftsize);

        d_fbuf.clear();
        d_fbuf.resize(d_fftsize);

        d_last_time = 0;
    }
}

void waterfall_sink_f_impl::check_clicked()
{
    if (d_main_gui->checkClicked()) {
        double freq = d_main_gui->getClickedFreq();
        message_port_pub(d_port, pmt::cons(d_port, pmt::from_double(freq)));
    }
}

void waterfall_sink_f_impl::handle_set_freq(pmt::pmt_t msg)
{
    if (pmt::is_pair(msg)) {
        pmt::pmt_t x = pmt::cdr(msg);
        if (pmt::is_real(x)) {
            d_center_freq = pmt::to_double(x);
            d_qApplication->postEvent(d_main_gui,
                                      new SetFreqEvent(d_center_freq, d_bandwidth));
        }
    }
}

void waterfall_sink_f_impl::handle_set_bw(pmt::pmt_t msg)
{
    if (pmt::is_pair(msg)) {
        pmt::pmt_t x = pmt::cdr(msg);
        if (pmt::is_real(x)) {
            d_bandwidth = pmt::to_double(x);
            d_qApplication->postEvent(d_main_gui,
                                      new SetFreqEvent(d_center_freq, d_bandwidth));
        }
    }
}

void waterfall_sink_f_impl::set_time_per_fft(double t) { d_main_gui->setTimePerFFT(t); }

int waterfall_sink_f_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    int j = 0;
    const float* in = (const float*)input_items[0];

    // Update the FFT size from the application
    fftresize();
    windowreset();
    check_clicked();

    for (int i = 0; i < noutput_items; i += d_fftsize) {
        unsigned int datasize = noutput_items - i;
        unsigned int resid = d_fftsize - d_index;

        // If we have enough input for one full FFT, do it
        if (datasize >= resid) {

            if (gr::high_res_timer_now() - d_last_time > d_update_time) {
                for (int n = 0; n < d_nconnections; n++) {
                    // Fill up residbuf with d_fftsize number of items
                    in = (const float*)input_items[n];
                    memcpy(
                        d_residbufs[n].data() + d_index, &in[j], sizeof(float) * resid);

                    fft(d_fbuf.data(), d_residbufs[n].data(), d_fftsize);
                    for (int x = 0; x < d_fftsize; x++) {
                        d_magbufs[n][x] = (double)((1.0 - d_fftavg) * d_magbufs[n][x] +
                                                   (d_fftavg)*d_fbuf[x]);
                    }
                    // volk_32f_convert_64f(d_magbufs[n], d_fbuf, d_fftsize);
                }

                d_last_time = gr::high_res_timer_now();
                d_qApplication->postEvent(
                    d_main_gui,
                    new WaterfallUpdateEvent(d_magbufs, d_fftsize, d_last_time));
            }

            d_index = 0;
            j += resid;
        }
        // Otherwise, copy what we received into the residbuf for next time
        else {
            for (int n = 0; n < d_nconnections; n++) {
                in = (const float*)input_items[n];
                memcpy(d_residbufs[n].data() + d_index, &in[j], sizeof(float) * datasize);
            }
            d_index += datasize;
            j += datasize;
        }
    }

    return j;
}

void waterfall_sink_f_impl::handle_pdus(pmt::pmt_t msg)
{
    size_t len;
    size_t start = 0;
    pmt::pmt_t dict, samples;

    // Test to make sure this is either a PDU or a uniform vector of
    // samples. Get the samples PMT and the dictionary if it's a PDU.
    // If not, we throw an error and exit.
    if (pmt::is_pair(msg)) {
        dict = pmt::car(msg);
        samples = pmt::cdr(msg);

        pmt::pmt_t start_key = pmt::string_to_symbol("start");
        if (pmt::dict_has_key(dict, start_key)) {
            start = pmt::to_uint64(pmt::dict_ref(dict, start_key, pmt::PMT_NIL));
        }
    } else if (pmt::is_uniform_vector(msg)) {
        samples = msg;
    } else {
        throw std::runtime_error("time_sink_c: message must be either "
                                 "a PDU or a uniform vector of samples.");
    }

    len = pmt::length(samples);

    const float* in;
    if (pmt::is_f32vector(samples)) {
        in = (const float*)pmt::f32vector_elements(samples, len);
    } else {
        throw std::runtime_error("waterfall sink: unknown data type "
                                 "of samples; must be float.");
    }

    // Plot if we're past the last update time
    if (gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();

        // Update the FFT size from the application
        fftresize();
        windowreset();
        check_clicked();

        gr::high_res_timer_type ref_start =
            (uint64_t)start * (double)(1.0 / d_bandwidth) * 1000000;

        int stride = std::max(0, (int)(len - d_fftsize) / (int)(d_nrows));

        set_time_per_fft(1.0 / d_bandwidth * stride);
        std::ostringstream title("");
        title << "Time (+" << (uint64_t)ref_start << "us)";
        set_time_title(title.str());

        int j = 0;
        size_t min = 0;
        size_t max = std::min(d_fftsize, static_cast<int>(len));
        while (j < d_nrows) {
            // Clear residbufs if len < d_fftsize
            memset(d_residbufs[d_nconnections].data(), 0x00, sizeof(float) * d_fftsize);

            // Copy in as much of the input samples as we can
            memcpy(d_residbufs[d_nconnections].data(),
                   &in[min],
                   sizeof(float) * (max - min));

            // Apply the window and FFT; copy data into the PDU
            // magnitude buffer.
            fft(d_fbuf.data(), d_residbufs[d_nconnections].data(), d_fftsize);
            for (int x = 0; x < d_fftsize; x++) {
                d_pdu_magbuf[j * d_fftsize + x] = (double)d_fbuf[x];
            }

            // Increment our indices; set max up to the number of
            // samples in the input PDU.
            min += stride;
            max = std::min(max + stride, len);
            j++;
        }

        // update gui per-pdu
        d_qApplication->postEvent(
            d_main_gui, new WaterfallUpdateEvent(d_magbufs, d_fftsize * d_nrows, 0));
    }
}

} /* namespace qtgui */
} /* namespace gr */
