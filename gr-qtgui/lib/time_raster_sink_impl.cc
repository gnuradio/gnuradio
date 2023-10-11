/* -*- c++ -*- */
/*
 * Copyright 2012,2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "time_raster_sink_impl.h"

#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>

#include <volk/volk.h>

#include <cstring>

namespace gr {
namespace qtgui {

template <class T>
typename time_raster_sink<T>::sptr
time_raster_sink<T>::make(double samp_rate,
                          double rows,
                          double cols,
                          const std::vector<float>& mult,
                          const std::vector<float>& offset,
                          const std::string& name,
                          int nconnections,
                          QWidget* parent)
{
    return gnuradio::make_block_sptr<time_raster_sink_impl<T>>(
        samp_rate, rows, cols, mult, offset, name, nconnections, parent);
}

template <class T>
time_raster_sink_impl<T>::time_raster_sink_impl(double samp_rate,
                                                double rows,
                                                double cols,
                                                const std::vector<float>& mult,
                                                const std::vector<float>& offset,
                                                const std::string& name,
                                                int nconnections,
                                                QWidget* parent)
    : sync_block("time_raster_sink",
                 io_signature::make(0, nconnections, sizeof(T)),
                 io_signature::make(0, 0, 0)),
      d_name(name),
      d_nconnections(nconnections),
      d_rows(rows),
      d_cols(cols),
      d_icols(static_cast<int>(ceil(d_cols))),
      d_tmpflt(d_icols),
      d_parent(parent),
      d_mult(std::vector<float>(nconnections + 1, 1)),
      d_offset(std::vector<float>(nconnections + 1, 0)),
      d_samp_rate(samp_rate)
{
    // setup PDU handling input port
    this->message_port_register_in(pmt::mp("in"));
    this->set_msg_handler(pmt::mp("in"),
                          [this](pmt::pmt_t msg) { this->handle_pdus(msg); });

    // +1 for the PDU buffer
    for (int i = 0; i < d_nconnections + 1; i++) {
        d_residbufs.emplace_back(d_icols);
    }

    this->set_multiplier(mult);
    this->set_offset(offset);

    this->initialize();
}

template <class T>
time_raster_sink_impl<T>::~time_raster_sink_impl()
{
    if (!d_main_gui->isClosed())
        d_main_gui->close();
}

template <class T>
bool time_raster_sink_impl<T>::check_topology(int ninputs, int noutputs)
{
    return ninputs == d_nconnections;
}

template <class T>
void time_raster_sink_impl<T>::initialize()
{
    if (qApp != NULL) {
        this->d_qApplication = qApp;
    } else {
        this->d_qApplication = new QApplication(d_argc, &d_argv);
    }

    // If a style sheet is set in the prefs file, enable it here.
    check_set_qss(this->d_qApplication);

    // Create time raster plot; as a bit input, we expect to see 1's
    // and 0's from each stream, so we set the maximum intensity
    // (zmax) to the number of connections so after adding the
    // streams, the max will be the max of 1's from all streams.
    int numplots = (d_nconnections > 0) ? d_nconnections : 1;
    d_main_gui =
        new TimeRasterDisplayForm(numplots, d_samp_rate, d_rows, d_cols, 1, d_parent);

    if (!d_name.empty())
        set_title(d_name);

    // initialize update time to 10 times a second
    set_update_time(0.1);
}

template <class T>
void time_raster_sink_impl<T>::exec_()
{
    this->d_qApplication->exec();
}

template <class T>
QWidget* time_raster_sink_impl<T>::qwidget()
{
    return d_main_gui;
}

template <class T>
void time_raster_sink_impl<T>::set_x_label(const std::string& label)
{
    d_main_gui->setXLabel(label);
}

template <class T>
void time_raster_sink_impl<T>::set_x_range(double start, double end)
{
    d_main_gui->setXAxis(start, end);
}

template <class T>
void time_raster_sink_impl<T>::set_y_label(const std::string& label)
{
    d_main_gui->setYLabel(label);
}

template <class T>
void time_raster_sink_impl<T>::set_y_range(double start, double end)
{
    d_main_gui->setYAxis(start, end);
}

template <class T>
void time_raster_sink_impl<T>::set_update_time(double t)
{
    // convert update time to ticks
    gr::high_res_timer_type tps = gr::high_res_timer_tps();
    d_update_time = t * tps;
    d_main_gui->setUpdateTime(t);
    d_last_time = 0;
}

template <class T>
void time_raster_sink_impl<T>::set_title(const std::string& title)
{
    d_main_gui->setTitle(title.c_str());
}

template <class T>
void time_raster_sink_impl<T>::set_line_label(unsigned int which,
                                              const std::string& label)
{
    d_main_gui->setLineLabel(which, label.c_str());
}

template <class T>
void time_raster_sink_impl<T>::set_line_color(unsigned int which,
                                              const std::string& color)
{
    d_main_gui->setLineColor(which, color.c_str());
}

template <class T>
void time_raster_sink_impl<T>::set_line_width(unsigned int which, int width)
{
    d_main_gui->setLineWidth(which, width);
}

template <class T>
void time_raster_sink_impl<T>::set_line_style(unsigned int which, Qt::PenStyle style)
{
    d_main_gui->setLineStyle(which, style);
}

template <class T>
void time_raster_sink_impl<T>::set_line_marker(unsigned int which,
                                               QwtSymbol::Style marker)
{
    d_main_gui->setLineMarker(which, marker);
}

template <class T>
void time_raster_sink_impl<T>::set_color_map(unsigned int which, const int color)
{
    d_main_gui->setColorMap(which, color);
}

template <class T>
void time_raster_sink_impl<T>::set_line_alpha(unsigned int which, double alpha)
{
    d_main_gui->setAlpha(which, (int)(255.0 * alpha));
}

template <class T>
void time_raster_sink_impl<T>::set_size(int width, int height)
{
    d_main_gui->resize(QSize(width, height));
}

template <class T>
void time_raster_sink_impl<T>::set_samp_rate(const double samp_rate)
{
    d_samp_rate = samp_rate;
    d_main_gui->setSampleRate(d_samp_rate);
}

template <class T>
void time_raster_sink_impl<T>::set_num_rows(double rows)
{
    gr::thread::scoped_lock lock(this->d_setlock);
    d_rows = rows;
    d_main_gui->setNumRows(rows);
}

template <class T>
void time_raster_sink_impl<T>::set_num_cols(double cols)
{
    if (d_cols != cols) {
        gr::thread::scoped_lock lock(this->d_setlock);

        this->d_qApplication->postEvent(d_main_gui, new TimeRasterSetSize(d_rows, cols));

        d_cols = cols;
        d_icols = static_cast<int>(ceil(d_cols));

        d_tmpflt.clear();
        d_tmpflt.resize(d_icols);

        for (int i = 0; i < d_nconnections + 1; i++) {
            d_residbufs[i].clear();
            d_residbufs[i].resize(d_icols);
        }
        reset();
    }
}

template <class T>
std::string time_raster_sink_impl<T>::title()
{
    return d_main_gui->title().toStdString();
}

template <class T>
std::string time_raster_sink_impl<T>::line_label(unsigned int which)
{
    return d_main_gui->lineLabel(which).toStdString();
}

template <class T>
std::string time_raster_sink_impl<T>::line_color(unsigned int which)
{
    return d_main_gui->lineColor(which).toStdString();
}

template <class T>
int time_raster_sink_impl<T>::line_width(unsigned int which)
{
    return d_main_gui->lineWidth(which);
}

template <class T>
int time_raster_sink_impl<T>::line_style(unsigned int which)
{
    return d_main_gui->lineStyle(which);
}

template <class T>
int time_raster_sink_impl<T>::line_marker(unsigned int which)
{
    return d_main_gui->lineMarker(which);
}

template <class T>
int time_raster_sink_impl<T>::color_map(unsigned int which)
{
    return d_main_gui->getColorMap(which);
}

template <class T>
double time_raster_sink_impl<T>::line_alpha(unsigned int which)
{
    return (double)(d_main_gui->markerAlpha(which)) / 255.0;
}

template <class T>
double time_raster_sink_impl<T>::num_rows()
{
    return d_main_gui->numRows();
}

template <class T>
double time_raster_sink_impl<T>::num_cols()
{
    return d_main_gui->numCols();
}

template <>
void time_raster_sink_impl<float>::set_multiplier(const std::vector<float>& mult)
{
    if (mult.empty()) {
        for (int i = 0; i < d_nconnections; i++) {
            d_mult[i] = 1.0f;
        }
    } else if (mult.size() == (size_t)d_nconnections) {
        for (int i = 0; i < d_nconnections; i++) {
            d_mult[i] = mult[i];
        }
    } else {
        throw std::runtime_error(
            "time_raster_sink_impl<T>::set_multiplier incorrect dimensions.");
    }
}

template <>
void time_raster_sink_impl<char>::set_multiplier(const std::vector<float>& mult)
{
    if (mult.empty()) {
        for (int i = 0; i < d_nconnections + 1; i++) {
            d_mult[i] = 1.0f;
        }
    } else if (mult.size() == (size_t)d_nconnections) {
        for (int i = 0; i < d_nconnections; i++) {
            d_mult[i] = mult[i];
        }
    } else if (mult.size() == (size_t)d_nconnections + 1) {
        for (int i = 0; i < d_nconnections + 1; i++) {
            d_mult[i] = mult[i];
        }
    } else {
        throw std::runtime_error(
            "time_raster_sink_impl<T>::set_multiplier incorrect dimensions.");
    }
}

template <>
void time_raster_sink_impl<float>::set_offset(const std::vector<float>& offset)
{
    if (offset.empty()) {
        for (int i = 0; i < d_nconnections; i++) {
            d_offset[i] = 0.0f;
        }
    } else if (offset.size() == (size_t)d_nconnections) {
        for (int i = 0; i < d_nconnections; i++) {
            d_offset[i] = offset[i];
        }
    } else {
        throw std::runtime_error(
            "time_raster_sink_impl<T>::set_offset incorrect dimensions.");
    }
}

template <>
void time_raster_sink_impl<char>::set_offset(const std::vector<float>& offset)
{
    if (offset.empty()) {
        for (int i = 0; i < d_nconnections + 1; i++) {
            d_offset[i] = 0.0f;
        }
    } else if (offset.size() == (size_t)d_nconnections) {
        for (int i = 0; i < d_nconnections; i++) {
            d_offset[i] = offset[i];
        }
    } else if (offset.size() == (size_t)d_nconnections + 1) {
        for (int i = 0; i < d_nconnections + 1; i++) {
            d_offset[i] = offset[i];
        }
    } else {
        throw std::runtime_error(
            "time_raster_sink_impl<T>::set_offset incorrect dimensions.");
    }
}

template <class T>
void time_raster_sink_impl<T>::set_intensity_range(float min, float max)
{
    d_main_gui->setIntensityRange(min, max);
}

template <class T>
void time_raster_sink_impl<T>::enable_menu(bool en)
{
    d_main_gui->enableMenu(en);
}

template <class T>
void time_raster_sink_impl<T>::enable_grid(bool en)
{
    d_main_gui->setGrid(en);
}

template <class T>
void time_raster_sink_impl<T>::enable_axis_labels(bool en)
{
    d_main_gui->setAxisLabels(en);
}

template <class T>
void time_raster_sink_impl<T>::enable_autoscale(bool en)
{
    d_main_gui->autoScale(en);
}

template <class T>
void time_raster_sink_impl<T>::reset()
{
    d_index = 0;
}

template <class T>
void time_raster_sink_impl<T>::_ncols_resize()
{
    double cols = d_main_gui->numCols();
    set_num_cols(cols);
}

template <>
int time_raster_sink_impl<float>::work(int noutput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    int n = 0, j = 0, idx = 0;
    const float* in = (const float*)input_items[0];

    _ncols_resize();

    for (int i = 0; i < noutput_items; i += d_icols) {
        unsigned int datasize = noutput_items - i;
        unsigned int resid = d_icols - d_index;
        idx = 0;

        // If we have enough input for one full plot, do it
        if (datasize >= resid) {

            // Fill up residbufs with d_size number of items
            for (n = 0; n < d_nconnections; n++) {
                in = (const float*)input_items[idx++];

                // Scale and add offset
                volk_32f_s32f_multiply_32f(d_tmpflt.data(), &in[j], d_mult[n], resid);
                for (unsigned int s = 0; s < resid; s++)
                    d_tmpflt[s] = d_tmpflt[s] + d_offset[n];

                volk_32f_convert_64f_u(&d_residbufs[n][d_index], d_tmpflt.data(), resid);
            }

            // Update the plot if its time
            if (gr::high_res_timer_now() - d_last_time > d_update_time) {
                d_last_time = gr::high_res_timer_now();
                this->d_qApplication->postEvent(
                    d_main_gui, new TimeRasterUpdateEvent(d_residbufs, d_cols));
            }

            d_index = 0;
            j += resid;
        }
        // Otherwise, copy what we received into the residbufs for next time
        // because we set the output_multiple, this should never need to be called
        else {
            for (n = 0; n < d_nconnections; n++) {
                in = (const float*)input_items[idx++];

                // Scale and add offset
                volk_32f_s32f_multiply_32f(d_tmpflt.data(), &in[j], d_mult[n], datasize);
                for (unsigned int s = 0; s < datasize; s++)
                    d_tmpflt[s] = d_tmpflt[s] + d_offset[n];

                volk_32f_convert_64f(&d_residbufs[n][d_index], d_tmpflt.data(), datasize);
            }
            d_index += datasize;
            j += datasize;
        }
    }

    return j;
}

template <>
int time_raster_sink_impl<char>::work(int noutput_items,
                                      gr_vector_const_void_star& input_items,
                                      gr_vector_void_star& output_items)
{
    int n = 0, j = 0, idx = 0;
    const int8_t* in = (const int8_t*)input_items[0];

    _ncols_resize();

    for (int i = 0; i < noutput_items; i += d_icols) {
        unsigned int datasize = noutput_items - i;
        unsigned int resid = d_icols - d_index;
        idx = 0;

        // If we have enough input for one full plot, do it
        if (datasize >= resid) {

            // Fill up residbufs with d_size number of items
            for (n = 0; n < d_nconnections; n++) {
                in = (const int8_t*)input_items[idx++];

                volk_8i_s32f_convert_32f(d_tmpflt.data(), &in[j], d_scale, resid);

                // Scale and add offset
                volk_32f_s32f_multiply_32f(
                    d_tmpflt.data(), d_tmpflt.data(), d_mult[n], resid);

                for (unsigned int s = 0; s < resid; s++)
                    d_tmpflt[s] = d_tmpflt[s] + d_offset[n];

                volk_32f_convert_64f_u(&d_residbufs[n][d_index], d_tmpflt.data(), resid);
            }

            // Update the plot if its time
            if (gr::high_res_timer_now() - d_last_time > d_update_time) {
                d_last_time = gr::high_res_timer_now();
                this->d_qApplication->postEvent(
                    d_main_gui, new TimeRasterUpdateEvent(d_residbufs, d_icols));
            }

            d_index = 0;
            j += resid;
        }
        // Otherwise, copy what we received into the residbufs for next time
        // because we set the output_multiple, this should never need to be called
        else {
            for (n = 0; n < d_nconnections; n++) {
                in = (const int8_t*)input_items[idx++];
                volk_8i_s32f_convert_32f(d_tmpflt.data(), &in[j], d_scale, datasize);
                // Scale and add offset
                volk_32f_s32f_multiply_32f(
                    d_tmpflt.data(), d_tmpflt.data(), d_mult[n], datasize);
                for (unsigned int s = 0; s < datasize; s++)
                    d_tmpflt[s] = d_tmpflt[s] + d_offset[n];

                volk_32f_convert_64f(&d_residbufs[n][d_index], d_tmpflt.data(), datasize);
            }
            d_index += datasize;
            j += datasize;
        }
    }

    return j;
}

template <>
void time_raster_sink_impl<float>::handle_pdus(pmt::pmt_t msg)
{
    size_t len;
    pmt::pmt_t dict, samples;

    // Test to make sure this is either a PDU or a uniform vector of
    // samples. Get the samples PMT and the dictionary if it's a PDU.
    // If not, we throw an error and exit.
    if (pmt::is_pair(msg)) {
        dict = pmt::car(msg);
        samples = pmt::cdr(msg);
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
        throw std::runtime_error("time_raster_sink<T>: unknown data type "
                                 "of samples; must be float.");
    }

    // Plot if we're past the last update time
    if (gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();

        _ncols_resize();

        d_rows = ceil(static_cast<double>(len) / static_cast<double>(d_cols));
        int irows = static_cast<int>(d_rows);

        this->d_qApplication->postEvent(d_main_gui,
                                        new TimeRasterSetSize(d_rows, d_cols));

        int idx = 0;
        for (int r = 0; r < irows; r++) {
            // Scale and add offset
            int cpy_len = std::min(static_cast<size_t>(d_cols), len - idx);
            memset(d_residbufs[d_nconnections].data(), 0, d_cols * sizeof(double));
            volk_32f_s32f_multiply_32f(
                d_tmpflt.data(), &in[idx], d_mult[d_nconnections], cpy_len);
            for (int c = 0; c < cpy_len; c++) {
                d_tmpflt[c] = d_tmpflt[c] + d_offset[d_nconnections];
            }

            volk_32f_convert_64f_u(
                d_residbufs[d_nconnections].data(), d_tmpflt.data(), cpy_len);

            this->d_qApplication->postEvent(
                d_main_gui, new TimeRasterUpdateEvent(d_residbufs, d_cols));
            idx += d_cols;
        }
    }
}

template <>
void time_raster_sink_impl<char>::handle_pdus(pmt::pmt_t msg)
{
    size_t len;
    pmt::pmt_t dict, samples;

    // Test to make sure this is either a PDU or a uniform vector of
    // samples. Get the samples PMT and the dictionary if it's a PDU.
    // If not, we throw an error and exit.
    if (pmt::is_pair(msg)) {
        dict = pmt::car(msg);
        samples = pmt::cdr(msg);
    } else if (pmt::is_uniform_vector(msg)) {
        samples = msg;
    } else {
        throw std::runtime_error("time_sink_c: message must be either "
                                 "a PDU or a uniform vector of samples.");
    }

    len = pmt::length(samples);

    const int8_t* in;
    if (pmt::is_s8vector(samples)) {
        in = (const int8_t*)pmt::s8vector_elements(samples, len);
    } else if (pmt::is_u8vector(samples)) {
        in = (const int8_t*)pmt::u8vector_elements(samples, len);
    } else {
        throw std::runtime_error("time_raster_sink<T>: unknown data type "
                                 "of samples; must be char ({u}int8_t).");
    }

    // Plot if we're past the last update time
    if (gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();

        _ncols_resize();

        d_rows = ceil(static_cast<double>(len) / static_cast<double>(d_cols));
        int irows = static_cast<int>(d_rows);

        this->d_qApplication->postEvent(d_main_gui,
                                        new TimeRasterSetSize(d_rows, d_cols));

        int idx = 0;
        for (int r = 0; r < irows; r++) {
            // Scale and add offset
            int cpy_len = std::min(static_cast<size_t>(d_cols), len - idx);
            memset(d_residbufs[d_nconnections].data(), 0, d_cols * sizeof(double));
            volk_8i_s32f_convert_32f(d_tmpflt.data(), &in[idx], d_scale, cpy_len);
            volk_32f_s32f_multiply_32f(
                d_tmpflt.data(), d_tmpflt.data(), d_mult[d_nconnections], cpy_len);
            for (int c = 0; c < cpy_len; c++) {
                d_tmpflt[c] = d_tmpflt[c] + d_offset[d_nconnections];
            }

            volk_32f_convert_64f_u(
                d_residbufs[d_nconnections].data(), d_tmpflt.data(), cpy_len);

            this->d_qApplication->postEvent(
                d_main_gui, new TimeRasterUpdateEvent(d_residbufs, d_cols));
            idx += d_cols;
        }
    }
}

template class time_raster_sink<float>;
template class time_raster_sink<char>;

} /* namespace qtgui */
} /* namespace gr */
