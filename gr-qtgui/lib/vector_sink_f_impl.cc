/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vector_sink_f_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>
#include <qwt_symbol.h>
#include <cmath>
#include <complex>
#include <cstring>

namespace gr {
namespace qtgui {

static const std::string MSG_PORT_OUT_XVAL = "xval";

// --- Helpers to handle type conversion ---
// For scalars, we preserve value (sign). For complex, we plot magnitude.

inline double get_plot_value(float v) { return (double)v; }
inline double get_plot_value(double v) { return v; }
inline double get_plot_value(int v) { return (double)v; }
inline double get_plot_value(short v) { return (double)v; }
inline double get_plot_value(unsigned char v) { return (double)v; }
inline double get_plot_value(std::complex<float> v) { return (double)std::abs(v); }


// --- Factory Implementations ---

vector_sink_f::sptr vector_sink_f::make(unsigned int vlen,
                                        double x_start,
                                        double x_step,
                                        const std::string& x_axis_label,
                                        const std::string& y_axis_label,
                                        const std::string& name,
                                        int nconnections,
                                        QWidget* parent)
{
    // Default Float Implementation
    return gnuradio::make_block_sptr<vector_sink_f_impl<float>>(
        vlen, x_start, x_step, x_axis_label, y_axis_label, name, nconnections, parent);
}

vector_sink_f::sptr vector_sink_f::make_c(unsigned int vlen,
                                          double x_start,
                                          double x_step,
                                          const std::string& x_axis_label,
                                          const std::string& y_axis_label,
                                          const std::string& name,
                                          int nconnections,
                                          QWidget* parent)
{
    return gnuradio::make_block_sptr<vector_sink_f_impl<std::complex<float>>>(
        vlen, x_start, x_step, x_axis_label, y_axis_label, name, nconnections, parent);
}

vector_sink_f::sptr vector_sink_f::make_i(unsigned int vlen,
                                          double x_start,
                                          double x_step,
                                          const std::string& x_axis_label,
                                          const std::string& y_axis_label,
                                          const std::string& name,
                                          int nconnections,
                                          QWidget* parent)
{
    return gnuradio::make_block_sptr<vector_sink_f_impl<int>>(
        vlen, x_start, x_step, x_axis_label, y_axis_label, name, nconnections, parent);
}

vector_sink_f::sptr vector_sink_f::make_s(unsigned int vlen,
                                          double x_start,
                                          double x_step,
                                          const std::string& x_axis_label,
                                          const std::string& y_axis_label,
                                          const std::string& name,
                                          int nconnections,
                                          QWidget* parent)
{
    return gnuradio::make_block_sptr<vector_sink_f_impl<short>>(
        vlen, x_start, x_step, x_axis_label, y_axis_label, name, nconnections, parent);
}

vector_sink_f::sptr vector_sink_f::make_b(unsigned int vlen,
                                          double x_start,
                                          double x_step,
                                          const std::string& x_axis_label,
                                          const std::string& y_axis_label,
                                          const std::string& name,
                                          int nconnections,
                                          QWidget* parent)
{
    return gnuradio::make_block_sptr<vector_sink_f_impl<unsigned char>>(
        vlen, x_start, x_step, x_axis_label, y_axis_label, name, nconnections, parent);
}


// --- Template Implementation ---

template <class T>
vector_sink_f_impl<T>::vector_sink_f_impl(unsigned int vlen,
                                          double x_start,
                                          double x_step,
                                          const std::string& x_axis_label,
                                          const std::string& y_axis_label,
                                          const std::string& name,
                                          int nconnections,
                                          QWidget* parent)
    : sync_block("vector_sink_f",
                 io_signature::make(1, -1, sizeof(T) * vlen),
                 io_signature::make(0, 0, 0)),
      d_vlen(vlen),
      d_vecavg(1.0),
      d_name(name),
      d_nconnections(nconnections),
      d_port(pmt::mp(MSG_PORT_OUT_XVAL)),
      d_msg(pmt::mp("x")),
      d_parent(parent)
{
    // setup output message port to post frequency when display is double-clicked
    message_port_register_out(d_port);

    for (int i = 0; i < d_nconnections; i++) {
        d_magbufs.emplace_back(d_vlen);
    }

    initialize(name, x_axis_label, y_axis_label, x_start, x_step);
}

template <class T>
vector_sink_f_impl<T>::~vector_sink_f_impl()
{
    QMetaObject::invokeMethod(d_main_gui, "close");
}

template <class T>
bool vector_sink_f_impl<T>::check_topology(int ninputs, int noutputs)
{
    return ninputs == d_nconnections;
}

template <class T>
void vector_sink_f_impl<T>::initialize(const std::string& name,
                                       const std::string& x_axis_label,
                                       const std::string& y_axis_label,
                                       double x_start,
                                       double x_step)
{
    if (qApp != NULL) {
        d_qApplication = qApp;
    } else {
        d_qApplication = new QApplication(d_argc, &d_argv);
    }

    // If a style sheet is set in the prefs file, enable it here.
    check_set_qss(d_qApplication);

    d_main_gui = new VectorDisplayForm(d_nconnections, d_parent);
    d_main_gui->setVecSize(d_vlen);
    set_x_axis(x_start, x_step);

    if (!name.empty())
        set_title(name);
    set_x_axis_label(x_axis_label);
    set_y_axis_label(y_axis_label);

    // initialize update time to 10 times a second
    set_update_time(0.1);
}

template <class T>
void vector_sink_f_impl<T>::exec_()
{
    d_qApplication->exec();
}

template <class T>
QWidget* vector_sink_f_impl<T>::qwidget()
{
    return d_main_gui;
}

template <class T>
unsigned int vector_sink_f_impl<T>::vlen() const
{
    return d_vlen;
}

template <class T>
void vector_sink_f_impl<T>::set_vec_average(const float avg)
{
    if (avg < 0 || avg > 1.0) {
        d_logger->alert("Invalid average value received in set_vec_average(), must be "
                        "within [0, 1].");
        return;
    }
    d_main_gui->setVecAverage(avg);
    d_vecavg = avg;
}

template <class T>
float vector_sink_f_impl<T>::vec_average() const
{
    return d_vecavg;
}

template <class T>
void vector_sink_f_impl<T>::set_x_axis(const double start, const double step)
{
    d_main_gui->setXaxis(start, step);
}

template <class T>
void vector_sink_f_impl<T>::set_y_axis(double min, double max)
{
    d_main_gui->setYaxis(min, max);
}

template <class T>
void vector_sink_f_impl<T>::set_ref_level(double ref_level)
{
    d_main_gui->setRefLevel(ref_level);
}

template <class T>
void vector_sink_f_impl<T>::set_x_axis_label(const std::string& label)
{
    d_main_gui->setXAxisLabel(label.c_str());
}

template <class T>
void vector_sink_f_impl<T>::set_y_axis_label(const std::string& label)
{
    d_main_gui->setYAxisLabel(label.c_str());
}

template <class T>
void vector_sink_f_impl<T>::set_x_axis_units(const std::string& units)
{
    d_main_gui->getPlot()->setXAxisUnit(units.c_str());
}

template <class T>
void vector_sink_f_impl<T>::set_y_axis_units(const std::string& units)
{
    d_main_gui->getPlot()->setYAxisUnit(units.c_str());
}

template <class T>
void vector_sink_f_impl<T>::set_update_time(double t)
{
    // convert update time to ticks
    gr::high_res_timer_type tps = gr::high_res_timer_tps();
    d_update_time = t * tps;
    d_main_gui->setUpdateTime(t);
    d_last_time = 0;
}

template <class T>
void vector_sink_f_impl<T>::set_title(const std::string& title)
{
    d_main_gui->setTitle(title.c_str());
}

template <class T>
void vector_sink_f_impl<T>::set_line_label(unsigned int which, const std::string& label)
{
    d_main_gui->setLineLabel(which, label.c_str());
}

template <class T>
void vector_sink_f_impl<T>::set_line_color(unsigned int which, const std::string& color)
{
    d_main_gui->setLineColor(which, color.c_str());
}

template <class T>
void vector_sink_f_impl<T>::set_line_width(unsigned int which, int width)
{
    d_main_gui->setLineWidth(which, width);
}

template <class T>
void vector_sink_f_impl<T>::set_line_style(unsigned int which, int style)
{
    d_main_gui->setLineStyle(which, (Qt::PenStyle)style);
}

template <class T>
void vector_sink_f_impl<T>::set_line_marker(unsigned int which, int marker)
{
    d_main_gui->setLineMarker(which, (QwtSymbol::Style)marker);
}

template <class T>
void vector_sink_f_impl<T>::set_line_alpha(unsigned int which, double alpha)
{
    d_main_gui->setMarkerAlpha(which, (int)(255.0 * alpha));
}

template <class T>
void vector_sink_f_impl<T>::set_size(int width, int height)
{
    d_main_gui->resize(QSize(width, height));
}

template <class T>
std::string vector_sink_f_impl<T>::title()
{
    return d_main_gui->title().toStdString();
}

template <class T>
std::string vector_sink_f_impl<T>::line_label(unsigned int which)
{
    return d_main_gui->lineLabel(which).toStdString();
}

template <class T>
std::string vector_sink_f_impl<T>::line_color(unsigned int which)
{
    return d_main_gui->lineColor(which).toStdString();
}

template <class T>
int vector_sink_f_impl<T>::line_width(unsigned int which)
{
    return d_main_gui->lineWidth(which);
}

template <class T>
int vector_sink_f_impl<T>::line_style(unsigned int which)
{
    return d_main_gui->lineStyle(which);
}

template <class T>
int vector_sink_f_impl<T>::line_marker(unsigned int which)
{
    return d_main_gui->lineMarker(which);
}

template <class T>
double vector_sink_f_impl<T>::line_alpha(unsigned int which)
{
    return (double)(d_main_gui->markerAlpha(which)) / 255.0;
}

template <class T>
void vector_sink_f_impl<T>::enable_menu(bool en)
{
    d_main_gui->enableMenu(en);
}

template <class T>
void vector_sink_f_impl<T>::enable_grid(bool en)
{
    d_main_gui->setGrid(en);
}

template <class T>
void vector_sink_f_impl<T>::disable_legend()
{
    d_main_gui->disableLegend();
}

template <class T>
void vector_sink_f_impl<T>::enable_autoscale(bool en)
{
    d_main_gui->autoScale(en);
}

template <class T>
void vector_sink_f_impl<T>::clear_max_hold()
{
    d_main_gui->clearMaxHold();
}

template <class T>
void vector_sink_f_impl<T>::clear_min_hold()
{
    d_main_gui->clearMinHold();
}

template <class T>
void vector_sink_f_impl<T>::reset()
{
    // nop
}

template <class T>
void vector_sink_f_impl<T>::check_clicked()
{
    if (d_main_gui->checkClicked()) {
        double xval = d_main_gui->getClickedXVal();
        message_port_pub(d_port, pmt::cons(d_msg, pmt::from_double(xval)));
    }
}

template <class T>
int vector_sink_f_impl<T>::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    // See if we generate a message
    check_clicked();

    for (int i = 0; i < noutput_items; i++) {
        if (gr::high_res_timer_now() - d_last_time > d_update_time) {
            for (int n = 0; n < d_nconnections; n++) {

                // POINTER MATH FIX:
                // input_items[n] points to start of this chunk buffer.
                // We access the i-th vector in the chunk.
                const T* in = (const T*)input_items[n] + (i * d_vlen);

                for (unsigned int x = 0; x < d_vlen; x++) {

                    double val = get_plot_value(in[x]);

                    d_magbufs[n][x] =
                        (double)((1.0 - d_vecavg) * d_magbufs[n][x] + (d_vecavg)*val);
                }
            }
            d_last_time = gr::high_res_timer_now();
            d_qApplication->postEvent(d_main_gui, new FreqUpdateEvent(d_magbufs, d_vlen));
        }
    }

    return noutput_items;
}

// Explicit instantiations to solve Symbol Not Found errors
template class vector_sink_f_impl<float>;
template class vector_sink_f_impl<std::complex<float>>;
template class vector_sink_f_impl<int>;
template class vector_sink_f_impl<short>;
template class vector_sink_f_impl<unsigned char>;

} /* namespace qtgui */
} /* namespace gr */
