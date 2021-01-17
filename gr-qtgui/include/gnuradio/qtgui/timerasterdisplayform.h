/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef TIMERASTER_DISPLAY_FORM_H
#define TIMERASTER_DISPLAY_FORM_H

#include <gnuradio/filter/firdes.h>
#include <gnuradio/qtgui/TimeRasterDisplayPlot.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <QtGui/QtGui>
#include <vector>

#include <gnuradio/qtgui/displayform.h>

/*!
 * \brief DisplayForm child for managing time raster plots.
 * \ingroup qtgui_blk
 */
class TimeRasterDisplayForm : public DisplayForm
{
    Q_OBJECT

public:
    TimeRasterDisplayForm(int nplots = 1,
                          double samp_rate = 1,
                          double rows = 1,
                          double cols = 1,
                          double zmax = 1,
                          QWidget* parent = 0);
    ~TimeRasterDisplayForm() override;

    TimeRasterDisplayPlot* getPlot() override;

    double numRows();
    double numCols();

    int getColorMap(unsigned int which);
    int getAlpha(unsigned int which);
    double getMinIntensity(unsigned int which);
    double getMaxIntensity(unsigned int which);

public slots:
    void customEvent(QEvent* e) override;

    void setNumRows(double rows);
    void setNumCols(double cols);

    void setNumRows(QString rows);
    void setNumCols(QString cols);

    void setSampleRate(const double samprate);
    void setSampleRate(const QString& rate) override;

    void setXAxis(double min, double max);
    void setXLabel(const std::string& label);
    void setYAxis(double min, double max);
    void setYLabel(const std::string& label);

    void setIntensityRange(const double minIntensity, const double maxIntensity);
    void setMaxIntensity(const QString& m);
    void setMinIntensity(const QString& m);

    void setColorMap(unsigned int which,
                     const int newType,
                     const QColor lowColor = QColor("white"),
                     const QColor highColor = QColor("white"));

    void setAlpha(unsigned int which, unsigned int alpha);

    void autoScale(bool en = false) override;

private slots:
    void newData(const QEvent* updateEvent) override;

private:
    double d_min_val, d_cur_min_val;
    double d_max_val, d_cur_max_val;
};

#endif /* TIMERASTER_DISPLAY_FORM_H */
