/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_MATRIX_SINK_IMPL_H
#define INCLUDED_QTGUI_MATRIX_SINK_IMPL_H

#include "matrix_display.h"
#include "matrix_display_signal.h"

#include <gnuradio/qtgui/matrix_sink.h>
#include <qapplication.h>
#include <QObject>
#include <vector>

namespace gr {
namespace qtgui {

class QTGUI_API matrix_sink_impl : public matrix_sink
{
private:
    QWidget* d_parent;
    unsigned int d_vlen;
    std::string d_name;
    std::vector<double> d_data;
    const char* d_argv = "";
    int d_argc = 1;
    QPointer<matrix_display> d_display;
    matrix_display_signal* d_signal = nullptr;

public:
    matrix_sink_impl(const std::string& name,
                     unsigned int num_cols,
                     unsigned int vlen,
                     bool contour,
                     const std::string& color_map,
                     const std::string& interpolation,
                     QWidget* parent = nullptr);
    ~matrix_sink_impl();

    void exec_() override;
    QApplication* d_qApplication;
    QWidget* qwidget() override;
    void set_x_start(double x_start) override;
    void set_x_end(double x_end) override;
    void set_y_start(double y_start) override;
    void set_y_end(double y_end) override;
    void set_z_max(double z_max) override;
    void set_z_min(double z_min) override;
    void set_x_axis_label(const std::string& x_axis_label) override;
    void set_y_axis_label(const std::string& y_axis_label) override;
    void set_z_axis_label(const std::string& z_axis_label) override;
    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_MATRIX_SINK_IMPL_H */
