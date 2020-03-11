/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef NUMBER_DISPLAY_FORM_H
#define NUMBER_DISPLAY_FORM_H

#include <gnuradio/qtgui/form_menus.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <QtGui/QtGui>
#include <qwt_thermo.h>
#include <vector>

/*!
 * \brief DisplayForm child for managing number sink plots.
 * \ingroup qtgui_blk
 */
class NumberDisplayForm : public QWidget
{
    Q_OBJECT

public:
    NumberDisplayForm(int nplots = 1,
                      gr::qtgui::graph_t type = gr::qtgui::NUM_GRAPH_HORIZ,
                      QWidget* parent = 0);
    ~NumberDisplayForm();

    gr::qtgui::graph_t graphType() const;
    QColor colorMin(unsigned int which) const;
    QColor colorMax(unsigned int which) const;
    std::string label(unsigned int which) const;
    float average() const;
    float updateTime() const;
    int scaleMin(unsigned int which);
    int scaleMax(unsigned int which);
    std::string title() const;
    std::string unit(unsigned int which) const;
    float factor(unsigned int which) const;

public slots:
    void mousePressEvent(QMouseEvent* e);
    void customEvent(QEvent* e);
    void setStop(bool on);
    void setStop();
    void setGraphType(const gr::qtgui::graph_t type);
    void setColor(unsigned int which, const QColor& min, const QColor& max);
    void setColorMin(unsigned int which, QString min);
    void setColorMax(unsigned int which, QString max);
    void setLabel(unsigned int which, const std::string& label);
    void setLabel(unsigned int which, QString label);
    void setAverage(const float avg);
    void setUpdateTime(const float time);
    void setUpdateTime(QString time);
    void saveFigure();
    void setScale(unsigned int which, int min, int max);
    void setScaleMin(unsigned int which, int min);
    void setScaleMax(unsigned int which, int max);
    void autoScale(bool on);
    void setTitle(const std::string& title);
    void setUnit(unsigned int which, const std::string& unit);
    void setFactor(unsigned int which, float factor);

private slots:
    void newData(const QEvent*);

private:
    unsigned int d_nplots;
    QGridLayout* d_layout;
    std::vector<QLabel*> d_label;
    std::vector<QLabel*> d_text_box;
    std::vector<QwtThermo*> d_indicator;
    gr::qtgui::graph_t d_graph_type;
    float d_avg, d_update_time;
    std::vector<float> d_max, d_min;

    bool d_menu_on;
    bool d_stop_state;
    bool d_autoscale_state;

    QMenu* d_menu;
    QAction* d_stop_act;
    QList<QMenu*> d_label_menu;
    std::vector<LineTitleAction*> d_label_act;
    std::vector<ItemFloatAct*> d_factor_act;
    FFTAverageMenu* d_avg_menu;
    NumberLayoutMenu* d_layout_menu;
    std::vector<NumberColorMapMenu*> d_color_menu;
    PopupMenu* d_maxcolor_menu;
    QAction* d_autoscale_act;
    PopupMenu* d_update_time_menu;
    QAction* d_save_act;

    QLabel* d_title;
    std::vector<std::string> d_unit;
    std::vector<float> d_factor;
};

#endif /* NUMBER_DISPLAY_FORM_H */
