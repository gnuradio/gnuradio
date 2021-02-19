/* -*- c++ -*- */
/*
 * Copyright 2008-2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sink_f_impl.h"

#include <gnuradio/fft/window.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>

#include <volk/volk.h>

#include <cstring>

namespace gr {
namespace qtgui {

namespace {
constexpr uint64_t maxBufferSize = 32768;
}

sink_f::sptr sink_f::make(int fftsize,
                          int wintype,
                          double fc,
                          double bw,
                          const std::string& name,
                          bool plotfreq,
                          bool plotwaterfall,
                          bool plottime,
                          bool plotconst,
                          QWidget* parent)
{
    return gnuradio::make_block_sptr<sink_f_impl>(fftsize,
                                                  wintype,
                                                  fc,
                                                  bw,
                                                  name,
                                                  plotfreq,
                                                  plotwaterfall,
                                                  plottime,
                                                  plotconst,
                                                  parent);
}

sink_f_impl::sink_f_impl(int fftsize,
                         int wintype,
                         double fc,
                         double bw,
                         const std::string& name,
                         bool plotfreq,
                         bool plotwaterfall,
                         bool plottime,
                         bool plotconst,
                         QWidget* parent)
    : block(
          "sink_f", io_signature::make(1, 1, sizeof(float)), io_signature::make(0, 0, 0)),
      d_fftsize(fftsize),
      d_wintype((fft::window::win_type)(wintype)),
      d_center_freq(fc),
      d_bandwidth(bw),
      d_name(name),
      d_port(pmt::mp("freq")),
      d_fft(std::make_unique<fft::fft_complex_fwd>(d_fftsize)),
      d_residbuf(d_fftsize),
      d_magbuf(d_fftsize),
      d_plotfreq(plotfreq),
      d_plotwaterfall(plotwaterfall),
      d_plottime(plottime),
      d_plotconst(plotconst),
      d_parent(parent),
      d_main_gui(maxBufferSize, d_fftsize, d_center_freq, -d_bandwidth, d_bandwidth)
{
    // setup output message port to post frequency when display is
    // double-clicked
    message_port_register_out(d_port);
    message_port_register_in(d_port);
    set_msg_handler(d_port, [this](pmt::pmt_t msg) { this->handle_set_freq(msg); });

    buildwindow();

    initialize();
}

sink_f_impl::~sink_f_impl() {}

bool sink_f_impl::check_topology(int ninputs, int noutputs) { return ninputs == 1; }

void sink_f_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    unsigned int ninputs = ninput_items_required.size();
    for (unsigned int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = std::min(d_fftsize, 8191);
    }
}

void sink_f_impl::initialize()
{
    if (qApp != NULL) {
        d_qApplication = qApp;
    } else {
#if QT_VERSION >= 0x040500 && QT_VERSION < 0x050000
        std::string style = prefs::singleton()->get_string("qtgui", "style", "raster");
        QApplication::setGraphicsSystem(QString(style.c_str()));
#endif
        d_qApplication = new QApplication(d_argc, &d_argv);
    }

    // If a style sheet is set in the prefs file, enable it here.
    check_set_qss(d_qApplication);

    d_main_gui.setDisplayTitle(d_name);
    d_main_gui.setWindowType((int)d_wintype);
    set_fft_size(d_fftsize);

    d_main_gui.openSpectrumWindow(
        d_parent, d_plotfreq, d_plotwaterfall, d_plottime, d_plotconst);

    // initialize update time to 10 times a second
    set_update_time(0.1);
}

void sink_f_impl::exec_() { d_qApplication->exec(); }

QWidget* sink_f_impl::qwidget() { return d_main_gui.qwidget(); }

#ifdef ENABLE_PYTHON
PyObject* sink_f_impl::pyqwidget()
{
    PyObject* w = PyLong_FromVoidPtr((void*)d_main_gui.qwidget());
    PyObject* retarg = Py_BuildValue("N", w);
    return retarg;
}
#else
void* sink_f_impl::pyqwidget() { return NULL; }
#endif

void sink_f_impl::set_fft_size(const int fftsize)
{
    d_fftsize = fftsize;
    d_main_gui.setFFTSize(fftsize);
}

int sink_f_impl::fft_size() const { return d_fftsize; }

void sink_f_impl::set_frequency_range(const double centerfreq, const double bandwidth)
{
    d_center_freq = centerfreq;
    d_bandwidth = bandwidth;
    d_main_gui.setFrequencyRange(d_center_freq, -d_bandwidth, d_bandwidth);
}

void sink_f_impl::set_fft_power_db(double min, double max)
{
    d_main_gui.setFrequencyAxis(min, max);
}

void sink_f_impl::enable_rf_freq(bool en) { d_main_gui.enableRFFreq(en); }

/*
void
sink_f_impl::set_time_domain_axis(double min, double max)
{
  d_main_gui.setTimeDomainAxis(min, max);
}

void
sink_f_impl::set_constellation_axis(double xmin, double xmax,
                                    double ymin, double ymax)
{
  d_main_gui.setConstellationAxis(xmin, xmax, ymin, ymax);
}

void
sink_f_impl::set_constellation_pen_size(int size)
{
  d_main_gui.setConstellationPenSize(size);
}
*/

void sink_f_impl::set_update_time(double t)
{
    d_update_time = t;
    d_main_gui.setUpdateTime(d_update_time);
}

void sink_f_impl::fft(float* data_out, const float* data_in, int size)
{
    if (!d_window.empty()) {
        gr_complex* dst = d_fft->get_inbuf();
        for (int i = 0; i < size; i++) // apply window
            dst[i] = data_in[i] * d_window[i];
    } else {
        gr_complex* dst = d_fft->get_inbuf();
        for (int i = 0; i < size; i++) // float to complex conversion
            dst[i] = data_in[i];
    }

    d_fft->execute(); // compute the fft
    volk_32fc_s32f_x2_power_spectral_density_32f(
        data_out, d_fft->get_outbuf(), size, 1.0, size);
}

void sink_f_impl::windowreset()
{
    fft::window::win_type newwintype;
    newwintype = (fft::window::win_type)d_main_gui.getWindowType();
    if (d_wintype != newwintype) {
        d_wintype = newwintype;
        buildwindow();
    }
}

void sink_f_impl::buildwindow()
{
    d_window.clear();
    if (d_wintype != 0) {
        d_window = filter::firdes::window(d_wintype, d_fftsize, 6.76);
    }
}

void sink_f_impl::fftresize()
{
    int newfftsize = d_main_gui.getFFTSize();

    if (newfftsize != d_fftsize) {

        // Resize residbuf and replace data
        d_residbuf.resize(newfftsize);
        d_magbuf.resize(newfftsize);

        // Set new fft size and reset buffer index
        // (throws away any currently held data, but who cares?)
        d_fftsize = newfftsize;
        d_index = 0;

        // Reset window to reflect new size
        buildwindow();

        // Reset FFTW plan for new size
        d_fft = std::make_unique<fft::fft_complex_fwd>(d_fftsize);
    }
}

void sink_f_impl::check_clicked()
{
    if (d_main_gui.checkClicked()) {
        double freq = d_main_gui.getClickedFreq();
        message_port_pub(d_port, pmt::cons(d_port, pmt::from_double(freq)));
    }
}

void sink_f_impl::handle_set_freq(pmt::pmt_t msg)
{
    if (pmt::is_pair(msg)) {
        pmt::pmt_t x = pmt::cdr(msg);
        if (pmt::is_real(x)) {
            d_center_freq = pmt::to_double(x);
            set_frequency_range(d_center_freq, d_bandwidth);
        }
    }
}

int sink_f_impl::general_work(int noutput_items,
                              gr_vector_int& ninput_items,
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
            const gr::high_res_timer_type currentTime = gr::high_res_timer_now();

            // Fill up residbuf with d_fftsize number of items
            memcpy(d_residbuf.data() + d_index, &in[j], sizeof(float) * resid);
            d_index = 0;

            j += resid;
            fft(d_magbuf.data(), d_residbuf.data(), d_fftsize);

            d_main_gui.updateWindow(true,
                                    d_magbuf.data(),
                                    d_fftsize,
                                    d_residbuf.data(),
                                    d_fftsize,
                                    NULL,
                                    0,
                                    currentTime,
                                    true);
        }
        // Otherwise, copy what we received into the residbuf for next time
        else {
            memcpy(d_residbuf.data() + d_index, &in[j], sizeof(float) * datasize);
            d_index += datasize;
            j += datasize;
        }
    }

    consume_each(j);
    return j;
}

} /* namespace qtgui */
} /* namespace gr */
