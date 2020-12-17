/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "ber_sink_b_impl.h"

#include <gnuradio/fft/fft.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <volk/volk.h>
#include <cmath>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

namespace gr {
namespace qtgui {

ber_sink_b::sptr ber_sink_b::make(std::vector<float> esnos,
                                  int curves,
                                  int ber_min_errors,
                                  float ber_limit,
                                  std::vector<std::string> curvenames,
                                  QWidget* parent)
{
    return gnuradio::make_block_sptr<ber_sink_b_impl>(
        esnos, curves, ber_min_errors, ber_limit, curvenames, parent);
}

ber_sink_b_impl::ber_sink_b_impl(std::vector<float> esnos,
                                 int curves,
                                 int ber_min_errors,
                                 float ber_limit,
                                 std::vector<std::string> curvenames,
                                 QWidget* parent)
    : block("ber_sink_b",
            io_signature::make(curves * esnos.size() * 2,
                               curves * esnos.size() * 2,
                               sizeof(unsigned char)),
            io_signature::make(0, 0, 0)),
      d_ber_min_errors(ber_min_errors),
      d_ber_limit(ber_limit),
      d_parent(parent),
      d_nconnections(esnos.size()),
      d_last_time(0)
{
    d_main_gui = NULL;

    // Enough curves for the input streams plus the BPSK AWGN curve.
    d_curves = curves;
    d_esno_buffers.reserve(curves + 1);
    d_ber_buffers.reserve(curves + 1);
    d_total.reserve(curves * esnos.size());
    d_total_errors.reserve(curves * esnos.size());

    for (int j = 0; j < curves; j++) {
        d_esno_buffers.push_back(
            (double*)volk_malloc(esnos.size() * sizeof(double), volk_get_alignment()));
        d_ber_buffers.push_back(
            (double*)volk_malloc(esnos.size() * sizeof(double), volk_get_alignment()));

        for (int i = 0; i < d_nconnections; i++) {
            d_esno_buffers[j][i] = esnos[i];
            d_ber_buffers[j][i] = 0.0;
            d_total.push_back(0);
            d_total_errors.push_back(1);
        }
    }

    // Now add the known curves
    d_esno_buffers.push_back(
        (double*)volk_malloc(esnos.size() * sizeof(double), volk_get_alignment()));
    d_ber_buffers.push_back(
        (double*)volk_malloc(esnos.size() * sizeof(double), volk_get_alignment()));
    for (size_t i = 0; i < esnos.size(); i++) {
        double e = pow(10.0, esnos[i] / 10.0);
        d_esno_buffers[curves][i] = esnos[i];
        d_ber_buffers[curves][i] = std::log10(0.5 * std::erf(std::sqrt(e)));
    }


    // Initialize and set up some of the curve visual properties
    initialize();
    for (int j = 0; j < curves; j++) {
        set_line_width(j, 1);
        // 35 unique styles supported
        set_line_style(j, (j % 5) + 1);
        set_line_marker(j, (j % 7));
    }

    if (curvenames.size() == (unsigned int)curves) {
        for (int j = 0; j < curves; j++) {
            if (!curvenames[j].empty()) {
                set_line_label(j, curvenames[j]);
            }
        }
    }

    set_line_label(d_curves, "BPSK AWGN");
    set_line_style(d_curves, 5);    // non-solid line
    set_line_marker(d_curves, -1);  // no marker
    set_line_alpha(d_curves, 0.25); // high transparency
}

ber_sink_b_impl::~ber_sink_b_impl()
{
    if (!d_main_gui->isClosed()) {
        d_main_gui->close();
    }

    for (unsigned int i = 0; i < d_esno_buffers.size(); i++) {
        volk_free(d_esno_buffers[i]);
        volk_free(d_ber_buffers[i]);
    }
}

bool ber_sink_b_impl::check_topology(int ninputs, int noutputs)
{
    return ninputs == (int)(d_curves * d_nconnections * 2);
}

void ber_sink_b_impl::initialize()
{
    if (qApp != NULL) {
        d_qApplication = qApp;
    } else {
        int argc = 0;
        char** argv = NULL;
        d_qApplication = new QApplication(argc, argv);
    }

    d_main_gui = new ConstellationDisplayForm(d_esno_buffers.size(), d_parent);

    d_main_gui->setNPoints(d_nconnections);
    d_main_gui->getPlot()->setAxisTitle(QwtPlot::yLeft, "LogScale BER");
    d_main_gui->getPlot()->setAxisTitle(QwtPlot::xBottom, "ESNO");
    // initialize update time to 10 times a second
    set_update_time(0.1);
}

void ber_sink_b_impl::exec_() { d_qApplication->exec(); }

QWidget* ber_sink_b_impl::qwidget() { return d_main_gui; }

#ifdef ENABLE_PYTHON
PyObject* ber_sink_b_impl::pyqwidget()
{
    PyObject* w = PyLong_FromVoidPtr((void*)d_main_gui);
    PyObject* retarg = Py_BuildValue("N", w);
    return retarg;
}
#else
void* ber_sink_b_impl::pyqwidget() { return NULL; }
#endif

void ber_sink_b_impl::set_y_axis(double min, double max)
{
    d_main_gui->setYaxis(min, max);
}

void ber_sink_b_impl::set_x_axis(double min, double max)
{
    d_main_gui->setXaxis(min, max);
}

void ber_sink_b_impl::set_update_time(double t)
{
    // convert update time to ticks
    gr::high_res_timer_type tps = gr::high_res_timer_tps();
    d_update_time = t * tps;
    d_main_gui->setUpdateTime(t);
    d_last_time = 0;
}

void ber_sink_b_impl::set_title(const std::string& title)
{
    d_main_gui->setTitle(title.c_str());
}

void ber_sink_b_impl::set_line_label(unsigned int which, const std::string& label)
{
    d_main_gui->setLineLabel(which, label.c_str());
}

void ber_sink_b_impl::set_line_color(unsigned int which, const std::string& color)
{
    d_main_gui->setLineColor(which, color.c_str());
}

void ber_sink_b_impl::set_line_width(unsigned int which, int width)
{
    d_main_gui->setLineWidth(which, width);
}

void ber_sink_b_impl::set_line_style(unsigned int which, int style)
{
    d_main_gui->setLineStyle(which, (Qt::PenStyle)style);
}

void ber_sink_b_impl::set_line_marker(unsigned int which, int marker)
{
    d_main_gui->setLineMarker(which, (QwtSymbol::Style)marker);
}

void ber_sink_b_impl::set_line_alpha(unsigned int which, double alpha)
{
    d_main_gui->setMarkerAlpha(which, (int)(255.0 * alpha));
}

void ber_sink_b_impl::set_size(int width, int height)
{
    d_main_gui->resize(QSize(width, height));
}

std::string ber_sink_b_impl::title() { return d_main_gui->title().toStdString(); }

std::string ber_sink_b_impl::line_label(unsigned int which)
{
    return d_main_gui->lineLabel(which).toStdString();
}

std::string ber_sink_b_impl::line_color(unsigned int which)
{
    return d_main_gui->lineColor(which).toStdString();
}

int ber_sink_b_impl::line_width(unsigned int which)
{
    return d_main_gui->lineWidth(which);
}

int ber_sink_b_impl::line_style(unsigned int which)
{
    return d_main_gui->lineStyle(which);
}

int ber_sink_b_impl::line_marker(unsigned int which)
{
    return d_main_gui->lineMarker(which);
}

double ber_sink_b_impl::line_alpha(unsigned int which)
{
    return (double)(d_main_gui->markerAlpha(which)) / 255.0;
}

int ber_sink_b_impl::nsamps() const { return d_nconnections; }

void ber_sink_b_impl::enable_menu(bool en) { d_main_gui->enableMenu(en); }

void ber_sink_b_impl::enable_autoscale(bool en) { d_main_gui->autoScale(en); }

int ber_sink_b_impl::general_work(int noutput_items,
                                  gr_vector_int& ninput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    if (gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();
        d_qApplication->postEvent(
            d_main_gui,
            new ConstUpdateEvent(d_esno_buffers, d_ber_buffers, d_nconnections));
    }

    // check stopping condition
    int done = 0, maxed = 0;
    for (int j = 0; j < d_curves; ++j) {
        for (int i = 0; i < d_nconnections; ++i) {

            if (d_total_errors[j * d_nconnections + i] >= d_ber_min_errors) {
                done++;
            } else if (std::log10(((double)d_ber_min_errors) /
                                  (d_total[j * d_nconnections + i] * 8.0)) <
                       d_ber_limit) {
                maxed++;
            }
        }
    }

    if (done + maxed == (int)(d_nconnections * d_curves)) {
        d_qApplication->postEvent(
            d_main_gui,
            new ConstUpdateEvent(d_esno_buffers, d_ber_buffers, d_nconnections));
        return -1;
    }

    float ber;
    for (unsigned int i = 0; i < ninput_items.size(); i += 2) {

        if ((d_total_errors[i >> 1] < d_ber_min_errors) &&
            (std::log10(((double)d_ber_min_errors) / (d_total[i >> 1] * 8.0)) >=
             d_ber_limit)) {

            int items = ninput_items[i] <= ninput_items[i + 1] ? ninput_items[i]
                                                               : ninput_items[i + 1];

            unsigned char* inbuffer0 = (unsigned char*)input_items[i];
            unsigned char* inbuffer1 = (unsigned char*)input_items[i + 1];

            if (items > 0) {
                uint32_t ret;
                for (int j = 0; j < items; j++) {
                    volk_32u_popcnt(&ret,
                                    static_cast<uint32_t>(inbuffer0[j] ^ inbuffer1[j]));
                    d_total_errors[i >> 1] += ret;
                }

                d_total[i >> 1] += items;

                ber = std::log10(((double)d_total_errors[i >> 1]) /
                                 (d_total[i >> 1] * 8.0));
                d_ber_buffers[i / (d_nconnections * 2)][(i % (d_nconnections * 2)) >> 1] =
                    ber;
            }
            consume(i, items);
            consume(i + 1, items);

            if (d_total_errors[i >> 1] >= d_ber_min_errors) {
                GR_LOG_INFO(d_logger,
                            boost::format("    %1% over %2%  -->  %3%") %
                                d_total_errors[i >> 1] % (d_total[i >> 1] * 8) % ber);
            } else if (std::log10(((double)d_ber_min_errors) / (d_total[i >> 1] * 8.0)) <
                       d_ber_limit) {
                GR_LOG_INFO(d_logger, "BER Limit Reached");
                d_ber_buffers[i / (d_nconnections * 2)][(i % (d_nconnections * 2)) >> 1] =
                    d_ber_limit;
                d_total_errors[i >> 1] = d_ber_min_errors + 1;
            }
        } else {
            consume(i, ninput_items[i]);
            consume(i + 1, ninput_items[i + 1]);
        }
    }

    return 0;
}


} /* namespace qtgui */
} /* namespace gr */
