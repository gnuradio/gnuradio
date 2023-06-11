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
                                    double x_start,
                                    double x_end,
                                    double y_start,
                                    double y_end,
                                    double z_max,
                                    double z_min,
                                    const std::string& x_axis_label,
                                    const std::string& y_axis_label,
                                    const std::string& z_axis_label,
                                    QWidget* parent)
{
    return gnuradio::make_block_sptr<matrix_sink_impl>(name,
                                                       num_cols,
                                                       vlen,
                                                       contour,
                                                       color_map,
                                                       interpolation,
                                                       x_start,
                                                       x_end,
                                                       y_start,
                                                       y_end,
                                                       z_max,
                                                       z_min,
                                                       x_axis_label,
                                                       y_axis_label,
                                                       z_axis_label,
                                                       parent);
}



matrix_sink_impl::matrix_sink_impl(const std::string& name,
                                   unsigned int num_cols,
                                   unsigned int vlen,
                                   bool contour,
                                   const std::string& color_map,
                                   const std::string& interpolation,
                                   double x_start,
                                   double x_end,
                                   double y_start,
                                   double y_end,
                                   double z_max,
                                   double z_min,
                                   const std::string& x_axis_label,
                                   const std::string& y_axis_label,
                                   const std::string& z_axis_label,
                                   QWidget* parent)
    : gr::sync_block("matrix_sink",
                     gr::io_signature::make(1, 1, sizeof(input_type) * vlen),
                     gr::io_signature::make(0, 0, 0)),
      d_vlen(vlen),
      d_name(name),
      d_parent(parent)
{
    d_display = nullptr;
    d_signal = nullptr;
    d_argv = nullptr;
    d_qApplication = nullptr;

    if (qApp != nullptr) {
        d_qApplication = qApp;
    } else {
        int argc = 1;
        d_argv = new char;
        d_argv[0] = '\0';
        d_qApplication = new QApplication(argc, &d_argv);
    }
    d_display = new matrix_display(name,
                                   num_cols,
                                   vlen,
                                   contour,
                                   color_map,
                                   interpolation,
                                   x_start,
                                   x_end,
                                   y_start,
                                   y_end,
                                   z_max,
                                   z_min,
                                   x_axis_label,
                                   y_axis_label,
                                   z_axis_label,
                                   d_parent);

    d_signal = new matrix_display_signal();

    qRegisterMetaType<QVector<double>>("QVector<double>");
    QObject::connect(d_signal,
                     &matrix_display_signal::data_ready,
                     d_display,
                     &matrix_display::set_data);
}

/*
 * Our virtual destructor.
 */
matrix_sink_impl::~matrix_sink_impl()
{
    // delete d_argv;
    if (!d_display->isClosed()) {
        d_display->close();
    }
    
}

void matrix_sink_impl::exec_() { d_qApplication->exec(); }

QWidget* matrix_sink_impl::qwidget() { return (QWidget*)d_display; }

int matrix_sink_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);


    QVector<double> qvec;
    for (unsigned int i = 0; i < d_vlen; i++) {
        qvec.append(in[i]);
    }


    emit d_signal->data_ready(qvec);

    return noutput_items;
}

} /* namespace qtgui */
} /* namespace gr */
