/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef CONSTELLATION_DISPLAY_FORM_H
#define CONSTELLATION_DISPLAY_FORM_H

#include <gnuradio/qtgui/ConstellationDisplayPlot.h>
#include <gnuradio/qtgui/api.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <QtGui/QtGui>
#include <vector>

#include <gnuradio/qtgui/displayform.h>

/*!
 * \brief DisplayForm child for managing constellaton (I&Q) plots.
 * \ingroup qtgui_blk
 */
class QTGUI_API ConstellationDisplayForm : public DisplayForm
{
    Q_OBJECT

public:
    ConstellationDisplayForm(int nplots = 1, QWidget* parent = 0);
    ~ConstellationDisplayForm();

    ConstellationDisplayPlot* getPlot();

    int getNPoints() const;
    gr::qtgui::trigger_mode getTriggerMode() const;
    gr::qtgui::trigger_slope getTriggerSlope() const;
    float getTriggerLevel() const;
    int getTriggerChannel() const;
    std::string getTriggerTagKey() const;

public slots:
    void customEvent(QEvent* e);
    void setNPoints(const int);

    void setSampleRate(const QString& samprate);
    void setYaxis(double min, double max);
    void setXaxis(double min, double max);
    void autoScale(bool en);

    void updateTrigger(gr::qtgui::trigger_mode mode);
    void setTriggerMode(gr::qtgui::trigger_mode mode);
    void setTriggerSlope(gr::qtgui::trigger_slope slope);
    void setTriggerLevel(QString s);
    void setTriggerLevel(float level);
    void setTriggerChannel(int chan);
    void setTriggerTagKey(QString s);
    void setTriggerTagKey(const std::string& s);

private slots:
    void newData(const QEvent*);

private:
    QIntValidator* d_int_validator;
    int d_npoints;

    NPointsMenu* d_nptsmenu;

    QMenu* d_triggermenu;
    TriggerModeMenu* d_tr_mode_menu;
    TriggerSlopeMenu* d_tr_slope_menu;
    PopupMenu* d_tr_level_act;
    TriggerChannelMenu* d_tr_channel_menu;
    PopupMenu* d_tr_tag_key_act;

    gr::qtgui::trigger_mode d_trig_mode;
    gr::qtgui::trigger_slope d_trig_slope;
    float d_trig_level;
    int d_trig_channel;
    std::string d_trig_tag_key;
};

#endif /* CONSTELLATION_DISPLAY_FORM_H */
