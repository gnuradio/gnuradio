/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "matrix_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <QVector>

namespace gr {
namespace qtgui {

using input_type = float;
matrix_sink::sptr matrix_sink::make(const std::string& name,
                                    unsigned int num_cols,
                                    unsigned int vlen,
                                    bool contour,
                                    const std::string& color_map,
                                    const std::string& interpolation,
                                    QWidget* parent)
{
    return gnuradio::make_block_sptr<matrix_sink_impl>(
        name, num_cols, vlen, contour, color_map, interpolation, parent);
}


matrix_sink_impl::matrix_sink_impl(const std::string& name,
                                   unsigned int num_cols,
                                   unsigned int vlen,
                                   bool contour,
                                   const std::string& color_map,
                                   const std::string& interpolation,
                                   QWidget* parent)
    : gr::sync_block("matrix_sink",
                     gr::io_signature::make(1, 1, sizeof(input_type) * vlen),
                     gr::io_signature::make(0, 0, 0)),
      d_parent(parent),
      d_vlen(vlen),
      d_name(name),
      d_display(new matrix_display(name,
                                   num_cols,
                                   vlen,
                                   contour,
                                   color_map,
                                   interpolation,
                                   0,
                                   1,
                                   0,
                                   1,
                                   0,
                                   1,
                                   "X",
                                   "Y",
                                   "Z",
                                   d_parent)),
      d_signal(new matrix_display_signal()),
      d_qApplication(qApp ? qApp : new QApplication(d_argc, const_cast<char**>(&d_argv)))
{
    qRegisterMetaType<QVector<double>>("QVector<double>");
    QObject::connect(d_signal,
                     &matrix_display_signal::data_ready,
                     d_display,
                     &matrix_display::set_data);
}

/*
 * Our virtual destructor.
 */
matrix_sink_impl::~matrix_sink_impl() { QMetaObject::invokeMethod(d_display, "close"); }

void matrix_sink_impl::exec_() { d_qApplication->exec(); }

QWidget* matrix_sink_impl::qwidget() { return (QWidget*)d_display; }

void matrix_sink_impl::set_x_axis_label(const std::string& x_axis_label)
{
    d_display->set_x_axis_label(x_axis_label);
}

void matrix_sink_impl::set_y_axis_label(const std::string& y_axis_label)
{
    d_display->set_y_axis_label(y_axis_label);
}

void matrix_sink_impl::set_z_axis_label(const std::string& z_axis_label)
{
    d_display->set_z_axis_label(z_axis_label);
}

void matrix_sink_impl::set_x_start(double x_start) { d_display->set_x_start(x_start); }

void matrix_sink_impl::set_x_end(double x_end) { d_display->set_x_end(x_end); }

void matrix_sink_impl::set_y_start(double y_start) { d_display->set_y_start(y_start); }

void matrix_sink_impl::set_y_end(double y_end) { d_display->set_y_end(y_end); }

void matrix_sink_impl::set_z_max(double z_max) { d_display->set_z_max(z_max); }

void matrix_sink_impl::set_z_min(double z_min) { d_display->set_z_min(z_min); }


int matrix_sink_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);

    for (int k = 0; k < noutput_items; k++) {
        QVector<double> qvec(d_vlen);
        std::copy(in + k * d_vlen, in + (k + 1) * d_vlen, qvec.begin());
        emit d_signal->data_ready(qvec);
    }

    return noutput_items;
}

} /* namespace qtgui */
} /* namespace gr */
