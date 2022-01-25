/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EYE_DISPLAY_FORM_H
#define EYE_DISPLAY_FORM_H

#include <gnuradio/qtgui/EyeDisplayPlot.h>
#include <gnuradio/qtgui/eyedisplaysform.h>
#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <QtGui/QtGui>
#include <vector>

class EyeControlPanel;

/*!
 * \brief EyeDisplaysForm child for managing eye pattern plots.
 * \ingroup qtgui_blk
 */
class EyeDisplayForm : public EyeDisplaysForm
{
    Q_OBJECT

public:
    EyeDisplayForm(int nplots = 1, bool cmplx = false, QWidget* parent = 0);
    ~EyeDisplayForm() override;

    EyeDisplayPlot* getPlot();
    EyeDisplayPlot* getSinglePlot(unsigned int);

    int getNPoints() const;
    int getSamplesPerSymbol() const;

    gr::qtgui::trigger_mode getTriggerMode() const;
    gr::qtgui::trigger_slope getTriggerSlope() const;
    float getTriggerLevel() const;
    float getTriggerDelay() const;
    int getTriggerChannel() const;
    std::string getTriggerTagKey() const;
    void setAxisLabels(bool en);

public slots:
    void customEvent(QEvent* e) override;

    void setSampleRate(const double samprate);
    void setSampleRate(const QString& samprate) override;
    void setYaxis(double min, double max);
    void setYLabel(const std::string& label, const std::string& unit = "");
    void setNPoints(const int);
    void autoScale(bool en) override;
    void autoScaleShot();
    void tagMenuSlot(bool en);
    void setTagMenu(unsigned int which, bool en);

    void updateTrigger(gr::qtgui::trigger_mode mode);
    void setTriggerMode(gr::qtgui::trigger_mode mode);
    void setTriggerSlope(gr::qtgui::trigger_slope slope);
    void setTriggerLevel(float level);
    void setTriggerDelay(float delay);
    void setTriggerChannel(int chan);
    void setTriggerTagKey(QString s);
    void setTriggerTagKey(const std::string& s);

    void setupControlPanel(bool en);
    void setupControlPanel();
    void teardownControlPanel();

private slots:
    void newData(const QEvent*) override;
    void notifyYAxisPlus();
    void notifyYAxisMinus();
    void notifyYRangePlus();
    void notifyYRangeMinus();
    void notifyTriggerMode(const QString& mode);
    void notifyTriggerSlope(const QString& slope);
    void notifyTriggerLevelPlus();
    void notifyTriggerLevelMinus();
    void notifyTriggerDelayPlus();
    void notifyTriggerDelayMinus();

signals:
    void signalTriggerMode(gr::qtgui::trigger_mode mode);
    void signalTriggerSlope(gr::qtgui::trigger_slope slope);
    void signalTriggerLevel(float level);
    void signalTriggerDelay(float delay);
    void signalReplot();
    void signalNPoints(const int npts);

private:
    double d_start_frequency;
    double d_stop_frequency;
    double d_current_units;
    int d_npoints;
    unsigned int d_rows;
    unsigned int d_cols;
    NPointsMenu* d_nptsmenu;

    QAction* d_controlpanelmenu;
    std::vector<QAction*> d_tagsmenu;

    QMenu* d_triggermenu;
    TriggerModeMenu* d_tr_mode_menu;
    TriggerSlopeMenu* d_tr_slope_menu;
    TriggerChannelMenu* d_tr_channel_menu;
    PopupMenu* d_tr_tag_key_act;

    gr::qtgui::trigger_mode d_trig_mode;
    gr::qtgui::trigger_slope d_trig_slope;
    float d_trig_level;
    float d_trig_delay;
    int d_trig_channel;
    std::string d_trig_tag_key;

    EyeControlPanel* d_controlpanel;
};

#endif /* EYE_DISPLAY_FORM_H */
