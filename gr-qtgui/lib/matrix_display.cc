/* -*- c++ -*- */
/*
 * Copyright 2023 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "matrix_display.h"
#include <QDebug>
#include <vector>

matrix_display::matrix_display(const std::string& name,
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
    : QWidget(parent),
      d_spectrogram(new QwtPlotSpectrogram()),
      d_plot(new QwtPlot(this)),
      d_vlen(vlen),
      d_num_cols(num_cols),
      d_x_start(x_start),
      d_x_end(x_end),
      d_y_start(y_start),
      d_y_end(y_end),
      d_z_max(z_max),
      d_z_min(z_min)
{
    set_color_map(color_map);

    set_x_axis_label(x_axis_label);
    set_y_axis_label(y_axis_label);
    set_z_axis_label(z_axis_label);


    d_spectrogram->setDisplayMode(QwtPlotSpectrogram::DisplayMode::ImageMode, true);
    set_contour(contour);
    d_spectrogram->setColorMap(d_colorMap);


    d_data.setInterval(Qt::XAxis, QwtInterval(d_x_start, d_x_end));
    d_data.setInterval(Qt::YAxis, QwtInterval(d_y_start, d_y_end));
    d_data.setInterval(Qt::ZAxis, QwtInterval(d_z_min, d_z_max));
    set_interpolation(interpolation);
    d_spectrogram->attach(d_plot);
    d_plot->repaint();
    d_plot->show();

    d_hLayout = new QHBoxLayout();
    d_hLayout->addWidget(d_plot);
    this->setLayout(d_hLayout);

    initialize_mouse_actions();
}
matrix_display::~matrix_display()
{
    // Qt deletes children when parent is deleted
    d_spectrogram->detach();
}

void matrix_display::closeEvent(QCloseEvent* e)
{
    qApp->processEvents();
    QWidget::closeEvent(e);
}

void matrix_display::initialize_mouse_actions()
{
    d_menu_on = true;
    d_menu = new QMenu(this);

    d_stop_act = new QAction("Stop", this);
    d_stop_act->setStatusTip(tr("Start/Stop"));
    connect(d_stop_act, SIGNAL(triggered()), this, SLOT(set_stop()));

    d_stop_state = false;
    d_menu->addAction(d_stop_act);

    d_save_act = new QAction("Save", this);
    d_save_act->setStatusTip(tr("Save Figure"));
    connect(d_save_act, SIGNAL(triggered()), this, SLOT(save_figure()));
    d_menu->addAction(d_save_act);

    d_contour_menu = new QMenu;
    d_contour_menu->setTitle("Contour");
    QAction* contour_on = new QAction("On", this);
    connect(contour_on, &QAction::triggered, this, [this]() { contour(true); });
    d_contour_menu->addAction(contour_on);
    QAction* contour_off = new QAction("Off", this);
    connect(contour_off, &QAction::triggered, this, [this]() { contour(false); });
    d_contour_menu->addAction(contour_off);
    d_menu->addMenu(d_contour_menu);

    d_interpolation_menu = new QMenu;
    d_interpolation_menu->setTitle("Interpolation");
    QAction* bilinear = new QAction("Bilinear Interpolation", this);
    connect(bilinear, &QAction::triggered, this, [this]() {
        interpolation("BilinearInterpolation");
    });
    d_interpolation_menu->addAction(bilinear);
    QAction* nearest = new QAction("Nearest Neighbour Interpolation", this);
    connect(nearest, &QAction::triggered, this, [this]() {
        interpolation("NearestNeighbour");
    });
    d_interpolation_menu->addAction(nearest);
    d_menu->addMenu(d_interpolation_menu);
}
void matrix_display::set_contour(bool contour)
{
    if (contour) {
        for (double level = 0.5; level < 10.0; level += 1.0)
            d_contour_levels += level;
        d_spectrogram->setContourLevels(d_contour_levels);
        d_spectrogram->setDisplayMode(QwtPlotSpectrogram::DisplayMode::ContourMode, true);
    } else
        d_spectrogram->setDisplayMode(QwtPlotSpectrogram::DisplayMode::ImageMode, true);
}

void matrix_display::set_color_map(const std::string& color_map)
{
    if (color_map == "rgb") {
        d_colorMap = new QwtLinearColorMap(Qt::darkCyan, Qt::red, QwtColorMap::RGB);
        d_colorMap->addColorStop(0.1, Qt::cyan);
        d_colorMap->addColorStop(0.6, Qt::green);
        d_colorMap->addColorStop(0.95, Qt::yellow);

        d_z_colorMap = new QwtLinearColorMap(Qt::darkCyan, Qt::red, QwtColorMap::RGB);
        d_z_colorMap->addColorStop(0.1, Qt::cyan);
        d_z_colorMap->addColorStop(0.6, Qt::green);
        d_z_colorMap->addColorStop(0.95, Qt::yellow);

    } else if (color_map == "indexed") {
        d_colorMap = new QwtLinearColorMap(Qt::blue, Qt::red, QwtColorMap::Indexed);

        d_z_colorMap = new QwtLinearColorMap(Qt::blue, Qt::red, QwtColorMap::Indexed);

    } else if (color_map == "alpha") {
        QwtAlphaColorMap* alpha_map = new QwtAlphaColorMap();
        alpha_map->setColor(Qt::darkBlue);
        d_colorMap = (QwtLinearColorMap*)alpha_map;
        d_z_colorMap = (QwtLinearColorMap*)alpha_map;
    }
}

void matrix_display::set_interpolation(const std::string& interpolation)
{
    //'BilinearInterpolation','NearestNeighbour'
    if (interpolation == "BilinearInterpolation") {
        d_data.setResampleMode(QwtMatrixRasterData::ResampleMode::BilinearInterpolation);
    } else if (interpolation == "NearestNeighbour") {
        d_data.setResampleMode(QwtMatrixRasterData::ResampleMode::NearestNeighbour);
    }
}

void matrix_display::set_x_start(double x_start)
{
    d_x_start = x_start;
    d_data.setInterval(Qt::XAxis, QwtInterval(d_x_start, d_x_end));
}

void matrix_display::set_x_end(double x_end)
{
    d_x_end = x_end;
    d_data.setInterval(Qt::XAxis, QwtInterval(d_x_start, d_x_end));
}

void matrix_display::set_y_start(double y_start)
{
    d_y_start = y_start;
    d_data.setInterval(Qt::YAxis, QwtInterval(d_y_start, d_y_end));
}

void matrix_display::set_y_end(double y_end)
{
    d_y_end = y_end;
    d_data.setInterval(Qt::YAxis, QwtInterval(d_y_start, d_y_end));
}

void matrix_display::set_z_max(double z_start)
{
    d_z_max = z_start;
    d_data.setInterval(Qt::ZAxis, QwtInterval(d_z_min, d_z_max));
}

void matrix_display::set_z_min(double z_end)
{
    d_z_min = z_end;
    d_data.setInterval(Qt::ZAxis, QwtInterval(d_z_min, d_z_max));
}

void matrix_display::set_x_axis_label(const std::string& x_axis_label)
{
    d_x_axis = d_plot->axisWidget(QwtPlot::xBottom);
    d_x_axis->setTitle(x_axis_label.c_str());
    d_plot->enableAxis(QwtPlot::xBottom, true);
}

void matrix_display::set_y_axis_label(const std::string& y_axis_label)
{
    d_y_axis = d_plot->axisWidget(QwtPlot::yLeft);
    d_y_axis->setTitle(y_axis_label.c_str());
    d_plot->enableAxis(QwtPlot::yLeft, true);
}

void matrix_display::set_z_axis_label(const std::string& z_axis_label)
{
    d_z_axis = d_plot->axisWidget(QwtPlot::yRight);
    d_z_axis->setTitle(z_axis_label.c_str());
    d_z_axis->setColorBarEnabled(true);
    d_z_axis->setColorMap(QwtInterval(d_z_min, d_z_max), d_z_colorMap);
    d_plot->setAxisScale(QwtPlot::yRight, d_z_min, d_z_max);
    d_plot->enableAxis(QwtPlot::yRight, true);
}


void matrix_display::save_figure()
{
    QPixmap qpix = this->grab();

    QString types = QString(tr("Portable Network Graphics file (*.png);;JPEG file "
                               "(*.jpg);;Bitmap file (*.bmp);;TIFF file (*.tiff)"));

    QString filename, filetype;
    QFileDialog* filebox = new QFileDialog(0, "Save Image", "./", types);
    filebox->setViewMode(QFileDialog::Detail);
    filebox->setAcceptMode(QFileDialog::AcceptSave);
    filebox->setFileMode(QFileDialog::AnyFile);
    if (filebox->exec()) {
        filename = filebox->selectedFiles()[0];
        filetype = filebox->selectedNameFilter();
    } else {
        return;
    }

    if (filetype.contains(".jpg")) {
        qpix.save(filename + ".jpg", "JPEG");
    } else if (filetype.contains(".png")) {
        qpix.save(filename + ".png", "PNG");
    } else if (filetype.contains(".bmp")) {
        qpix.save(filename + ".bmp", "BMP");
    } else if (filetype.contains(".tiff")) {
        qpix.save(filename + ".tiff", "TIFF");
    } else {
        qpix.save(filename + ".jpg", "JPEG");
    }

    delete filebox;
}

void matrix_display::mousePressEvent(QMouseEvent* e)
{

    bool ctrloff = Qt::ControlModifier != QApplication::keyboardModifiers();
    if ((e->button() == Qt::MiddleButton) && ctrloff && (d_menu_on)) {
        if (d_stop_state == false)
            d_stop_act->setText(tr("Stop"));
        else
            d_stop_act->setText(tr("Start"));
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)

        d_menu->exec(e->globalPos());
#else
        d_menu->exec(e->globalPosition().toPoint());
#endif
    }
}

void matrix_display::set_stop(bool on)
{
    if (!on) {
        d_stop_state = false;
    } else {
        d_stop_state = true;
    }
}

void matrix_display::set_stop()
{
    if (d_stop_state == false)
        set_stop(true);
    else
        set_stop(false);
}

void matrix_display::contour(bool contour)
{
    if (contour) {
        for (double level = 0.5; level < 10.0; level += 1.0)
            d_contour_levels += level;
        d_spectrogram->setContourLevels(d_contour_levels);
        d_spectrogram->setDisplayMode(QwtPlotSpectrogram::DisplayMode::ContourMode, true);
    } else {
        d_spectrogram->setDisplayMode(QwtPlotSpectrogram::DisplayMode::ContourMode,
                                      false);
        d_spectrogram->setDisplayMode(QwtPlotSpectrogram::DisplayMode::ImageMode, true);
    }

    d_plot->replot();
}


void matrix_display::interpolation(const std::string& interpolation)
{
    //'BilinearInterpolation','NearestNeighbour'
    if (interpolation == "BilinearInterpolation") {
        d_data.setResampleMode(QwtMatrixRasterData::ResampleMode::BilinearInterpolation);
    } else if (interpolation == "NearestNeighbour") {
        d_data.setResampleMode(QwtMatrixRasterData::ResampleMode::NearestNeighbour);
    }

    d_plot->replot();
}

void matrix_display::set_data(QVector<double> data)
{

    if (!d_stop_state) {
        d_data.setValueMatrix(data, d_num_cols);

        d_spectrogram->setData(&d_data);
        d_plot->replot();
    }
}
