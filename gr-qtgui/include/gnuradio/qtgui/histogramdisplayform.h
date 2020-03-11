/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef HISTOGRAM_DISPLAY_FORM_H
#define HISTOGRAM_DISPLAY_FORM_H

#include <gnuradio/qtgui/HistogramDisplayPlot.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <QtGui/QtGui>
#include <vector>

#include <gnuradio/qtgui/displayform.h>

/*!
 * \brief DisplayForm child for managing histogram domain plots.
 * \ingroup qtgui_blk
 */
class HistogramDisplayForm : public DisplayForm
{
    Q_OBJECT

public:
    HistogramDisplayForm(int nplots = 1, QWidget* parent = 0);
    ~HistogramDisplayForm();

    HistogramDisplayPlot* getPlot();

    int getNPoints() const;

public slots:
    void customEvent(QEvent* e);

    void setYaxis(double min, double max);
    void setXaxis(double min, double max);
    void setNPoints(const int);
    void autoScale(bool en);
    void setSemilogx(bool en);
    void setSemilogy(bool en);

    void setNumBins(const int);
    void setAccumulate(bool en);
    bool getAccumulate();
    void autoScaleX();

private slots:
    void newData(const QEvent*);

private:
    QIntValidator* d_int_validator;

    double d_startFrequency;
    double d_stopFrequency;

    int d_npoints;

    bool d_semilogx;
    bool d_semilogy;

    NPointsMenu* d_nptsmenu;
    NPointsMenu* d_nbinsmenu;
    QAction* d_semilogxmenu;
    QAction* d_semilogymenu;

    QAction* d_accum_act;
    QAction* d_autoscalex_act;
    bool d_autoscalex_state;
};

#endif /* HISTOGRAM_DISPLAY_FORM_H */
