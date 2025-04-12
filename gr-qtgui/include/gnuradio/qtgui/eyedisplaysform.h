/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EYE_DISPLAYS_FORM_H
#define EYE_DISPLAYS_FORM_H

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
class QTGUI_API EyeDisplaysForm : public QWidget
{
    Q_OBJECT

public:
    EyeDisplaysForm(int nplots = 1, QWidget* parent = 0);
    ~EyeDisplaysForm() override;

    void Reset();

    void enableMenu(bool en = true);

public slots:
    void resizeEvent(QResizeEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void customEvent(QEvent* e) override = 0;

    void closeEvent(QCloseEvent* e) override;

    void setUpdateTime(double t);
    void setSamplesPerSymbol(int64_t sps);
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
    unsigned int d_nplots;
    int d_sps;

    QGridLayout* d_layout;
    DisplayPlot* d_display_plot;
    std::vector<DisplayPlot*> d_displays_plot;
    bool d_system_specified_flag;

    std::vector<QwtPlotGrid*> d_grids;

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

#endif /* EYE_DISPLAYS_FORM_H */
