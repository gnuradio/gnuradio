/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_MATRIX_SINK_H
#define INCLUDED_QTGUI_MATRIX_SINK_H

#include <gnuradio/qtgui/api.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>
#include <QWidget>

namespace gr {
namespace qtgui {

/*!
 * \brief A graphical sink that displays a matrix.
 * \ingroup qtgui
 * \ingroup qtgui_blk
 *
 * \details
 * This block displays a matrix as a 2D plot. The matrix is
 * displayed as a contour plot, with the color of each contour
 * determined by the value of the matrix at that point.
 *
 * The matrix is displayed as a 2D plot, with the x and y axes
 * corresponding to the row and column indices of the matrix,
 * respectively. The z axis corresponds to the value of the
 * matrix at that point.
 *
 */
class QTGUI_API matrix_sink : virtual public gr::sync_block
{
public:
    // gr::qtgui::matrix_sink::sptr
    typedef std::shared_ptr<matrix_sink> sptr;

    /*!
     * \brief Build a matrix sink block.
     *
     * \param name The name of the block.
     * \param num_cols The number of columns in the matrix.
     * \param vlen The vector length of the matrix.
     * \param contour Whether or not to display the matrix as a contour plot.
     * \param color_map The color map to use for the contour plot.
     * \param interpolation The interpolation method to use for the contour plot.
     * \param parent The parent QWidget.
     */
    static sptr make(const std::string& name,
                     unsigned int num_cols,
                     unsigned int vlen,
                     bool contour,
                     const std::string& color_map,
                     const std::string& interpolation,
                     QWidget* parent = nullptr);

    virtual void exec_() = 0;
    virtual QWidget* qwidget() = 0;
    virtual void set_x_start(double x_start) = 0;
    virtual void set_x_end(double x_end) = 0;
    virtual void set_y_start(double y_start) = 0;
    virtual void set_y_end(double y_end) = 0;
    virtual void set_z_max(double z_max) = 0;
    virtual void set_z_min(double z_min) = 0;
    virtual void set_x_axis_label(const std::string& x_axis_label) = 0;
    virtual void set_y_axis_label(const std::string& y_axis_label) = 0;
    virtual void set_z_axis_label(const std::string& z_axis_label) = 0;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_MATRIX_SINK_H */
