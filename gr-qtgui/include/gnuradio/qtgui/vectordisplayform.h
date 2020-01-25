/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef VECTOR_DISPLAY_FORM_H
#define VECTOR_DISPLAY_FORM_H

#include <gnuradio/qtgui/VectorDisplayPlot.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <QtGui/QtGui>
#include <vector>

#include <gnuradio/qtgui/displayform.h>

/*!
 * \brief DisplayForm child for managing vector plots.
 * \ingroup qtgui_blk
 */
class VectorDisplayForm : public DisplayForm
{
    Q_OBJECT

public:
    VectorDisplayForm(int nplots = 1, QWidget* parent = 0);
    ~VectorDisplayForm();

    VectorDisplayPlot* getPlot();

    int getVecSize() const;
    float getVecAverage() const;

    void setXAxisLabel(const QString& label);
    void setYAxisLabel(const QString& label);
    void setRefLevel(double refLevel);

    // returns the x-Value that was last double-clicked on by the user
    float getClickedXVal() const;

    // checks if there was a double-click event; reset if there was
    bool checkClicked();

public slots:
    void customEvent(QEvent* e);

    void setVecSize(const int);
    void setVecAverage(const float);

    void setXaxis(double start, double step);
    void setYaxis(double min, double max);
    void setYMax(const QString& m);
    void setYMin(const QString& m);
    void autoScale(bool en);
    void clearMaxHold();
    void clearMinHold();

private slots:
    void newData(const QEvent* updateEvent);
    void onPlotPointSelected(const QPointF p);

private:
    uint64_t d_num_real_data_points;
    QIntValidator* d_int_validator;

    int d_vecsize;
    float d_vecavg;
    double d_ref_level;

    // TODO do I keep these?
    bool d_clicked;
    double d_clicked_x_level;

    AverageMenu* d_avgmenu;
    QAction *d_clearmin_act, *d_clearmax_act;
};

#endif /* VECTOR_DISPLAY_FORM_H */
