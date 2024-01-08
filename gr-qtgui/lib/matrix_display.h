/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MATRIX_DISPLAY_H
#define MATRIX_DISPLAY_H

#include <gnuradio/qtgui/form_menus.h>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <qwt_color_map.h>
#include <qwt_interval.h>
#include <qwt_matrix_raster_data.h>
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QPixmap>
#include <QVector>
#include <vector>

/*!
 * \brief Matrix Display child for managing sink plot.
 * \ingroup qtgui_blk
 */
class matrix_display : public QWidget
{
    Q_OBJECT

    QwtPlotSpectrogram* d_spectrogram;
    QwtPlot* d_plot;
    QwtLinearColorMap* d_colorMap;
    QwtMatrixRasterData d_data;
    QwtScaleWidget* d_x_axis;
    QwtScaleWidget* d_y_axis;
    QwtScaleWidget* d_z_axis;
    QwtLinearColorMap* d_z_colorMap;

    QHBoxLayout* d_hLayout;


    unsigned int d_vlen;
    unsigned int d_num_cols;
    double d_x_start;
    double d_x_end;
    double d_y_start;
    double d_y_end;
    double d_z_max;
    double d_z_min;
    QList<double> d_contour_levels;
    QMenu* d_menu = nullptr;
    QAction* d_save_act = nullptr;
    bool d_menu_on;
    bool d_stop_state;
    QAction* d_stop_act = nullptr;
    QMenu* d_contour_menu = nullptr;
    QMenu* d_interpolation_menu = nullptr;
    QMenu* d_color_map_menu = nullptr;

public:
    explicit matrix_display(const std::string& name,
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
                            QWidget* parent = nullptr);
    ~matrix_display();

    void set_contour(bool contour);
    void set_color_map(const std::string& color_map);
    void set_interpolation(const std::string& interpolation);
    void set_x_start(double x_start);
    void set_x_end(double x_end);
    void set_y_start(double y_start);
    void set_y_end(double y_end);
    void set_z_max(double z_max);
    void set_z_min(double z_min);
    void set_x_axis_label(const std::string& x_axis_label);
    void set_y_axis_label(const std::string& y_axis_label);
    void set_z_axis_label(const std::string& z_axis_label);
    void initialize_mouse_actions();

public slots:
    void set_data(QVector<double> data);
    void save_figure();
    void mousePressEvent(QMouseEvent* e) override;
    void set_stop(bool on);
    void set_stop();
    void contour(bool contour);
    void interpolation(const std::string& interpolation);
    void closeEvent(QCloseEvent* e) override;
};
#endif // MATRIX_DISPLAY_H
