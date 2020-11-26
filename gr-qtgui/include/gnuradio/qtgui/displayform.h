/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef DISPLAY_FORM_H
#define DISPLAY_FORM_H

#include <gnuradio/qtgui/api.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <QtGui/QtGui>
#include <vector>

#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>

#include <gnuradio/qtgui/DisplayPlot.h>
#include <gnuradio/qtgui/form_menus.h>

/*!
 * \brief Base class for setting up and  managing QTGUI plot forms.
 * \ingroup qtgui_blk
 */
class QTGUI_API DisplayForm : public QWidget
{
    Q_OBJECT

public:
    DisplayForm(int nplots = 1, QWidget* parent = 0);
    ~DisplayForm() override;

    virtual DisplayPlot* getPlot() = 0;
    void Reset();
    bool isClosed() const;

    void enableMenu(bool en = true);

public slots:
    void mousePressEvent(QMouseEvent* e) override;
    void customEvent(QEvent* e) override = 0;

    void closeEvent(QCloseEvent* e) override;

    void setUpdateTime(double t);

    void setTitle(const QString& title);
    void setLineLabel(unsigned int which, const QString& label);
    void setLineColor(unsigned int which, const QString& color);
    void setLineWidth(unsigned int which, unsigned int width);
    void setLineStyle(unsigned int which, Qt::PenStyle style);
    void setLineMarker(unsigned int which, QwtSymbol::Style style);
    void setMarkerAlpha(unsigned int which, unsigned int alpha);

    QString title();
    QString lineLabel(unsigned int which);
    QString lineColor(unsigned int which);
    int lineWidth(unsigned int which);
    Qt::PenStyle lineStyle(unsigned int which);
    QwtSymbol::Style lineMarker(unsigned int which);
    int markerAlpha(unsigned int which);

    virtual void setSampleRate(const QString& rate);

    void setStop(bool on);
    void setStop();

    void setGrid(bool on);
    void setAxisLabels(bool en);

    void saveFigure();

    void disableLegend();

private slots:
    virtual void newData(const QEvent*) = 0;
    virtual void autoScale(bool) = 0;
    void updateGuiTimer();

    virtual void onPlotPointSelected(const QPointF p);

signals:
    void plotPointSelected(const QPointF p, int type);
    void toggleGrid(bool en);

protected:
    bool d_isclosed;

    unsigned int d_nplots;

    QGridLayout* d_layout;
    DisplayPlot* d_display_plot;
    bool d_system_specified_flag;

    QwtPlotGrid* d_grid;

    bool d_menu_on;
    QMenu* d_menu;

    QAction* d_stop_act;
    bool d_stop_state;
    QAction* d_grid_act;
    bool d_grid_state;
    QAction* d_axislabelsmenu;
    bool d_axislabels;

    QAction* d_autoscale_act;
    bool d_autoscale_state;

    QList<QMenu*> d_lines_menu;
    QList<LineTitleAction*> d_line_title_act;
    QList<LineColorMenu*> d_line_color_menu;
    QList<LineWidthMenu*> d_line_width_menu;
    QList<LineStyleMenu*> d_line_style_menu;
    QList<LineMarkerMenu*> d_line_marker_menu;
    QList<MarkerAlphaMenu*> d_marker_alpha_menu;

    PopupMenu* d_samp_rate_act;
    QAction* d_save_act;

    double d_update_time;
};

#endif /* DISPLAY_FORM_H */
