/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef TIME_DISPLAY_FORM_H
#define TIME_DISPLAY_FORM_H

#include <gnuradio/qtgui/spectrumUpdateEvents.h>
#include <gnuradio/qtgui/TimeDomainDisplayPlot.h>
#include <gnuradio/qtgui/displayform.h>
#include <QtGui/QtGui>
#include <vector>

class TimeControlPanel;

/*!
 * \brief DisplayForm child for managing time domain plots.
 * \ingroup qtgui_blk
 */
class TimeDisplayForm : public DisplayForm
{
  Q_OBJECT

public:
  TimeDisplayForm(int nplots=1, QWidget* parent = 0);
  ~TimeDisplayForm();

  TimeDomainDisplayPlot* getPlot();

  int getNPoints() const;
  gr::qtgui::trigger_mode getTriggerMode() const;
  gr::qtgui::trigger_slope getTriggerSlope() const;
  float getTriggerLevel() const;
  float getTriggerDelay() const;
  int getTriggerChannel() const;
  std::string getTriggerTagKey() const;

public slots:
  void customEvent(QEvent * e);

  void setSampleRate(const double samprate);
  void setSampleRate(const QString &samprate);
  void setYaxis(double min, double max);
  void setYLabel(const std::string &label,
                 const std::string &unit="");
  void setNPoints(const int);
  void setStem(bool en);
  void autoScale(bool en);
  void autoScaleShot();
  void setSemilogx(bool en);
  void setSemilogy(bool en);
  void tagMenuSlot(bool en);
  void setTagMenu(int which, bool en);

  void updateTrigger(gr::qtgui::trigger_mode mode);
  void setTriggerMode(gr::qtgui::trigger_mode mode);
  void setTriggerSlope(gr::qtgui::trigger_slope slope);
  void setTriggerLevel(QString s);
  void setTriggerLevel(float level);
  void setTriggerDelay(QString s);
  void setTriggerDelay(float delay);
  void setTriggerChannel(int chan);
  void setTriggerTagKey(QString s);
  void setTriggerTagKey(const std::string &s);

  void setupControlPanel(bool en);
  void setupControlPanel();
  void teardownControlPanel();

private slots:
  void newData(const QEvent*);
  void notifyYAxisPlus();
  void notifyYAxisMinus();
  void notifyYRangePlus();
  void notifyYRangeMinus();
  void notifyXAxisPlus();
  void notifyXAxisMinus();
  void notifyTriggerMode(const QString &mode);
  void notifyTriggerSlope(const QString &slope);
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
  QIntValidator* d_int_validator;

  double d_start_frequency;
  double d_stop_frequency;
  double d_current_units;

  int d_npoints;

  bool d_stem;
  bool d_semilogx;
  bool d_semilogy;

  NPointsMenu *d_nptsmenu;
  QAction *d_stemmenu;
  QAction *d_semilogxmenu;
  QAction *d_semilogymenu;
  QAction *d_controlpanelmenu;
  std::vector<QAction*> d_tagsmenu;

  QMenu *d_triggermenu;
  TriggerModeMenu *d_tr_mode_menu;
  TriggerSlopeMenu *d_tr_slope_menu;
  PopupMenu *d_tr_level_act;
  PopupMenu *d_tr_delay_act;
  TriggerChannelMenu *d_tr_channel_menu;
  PopupMenu *d_tr_tag_key_act;

  gr::qtgui::trigger_mode d_trig_mode;
  gr::qtgui::trigger_slope d_trig_slope;
  float d_trig_level;
  float d_trig_delay;
  int d_trig_channel;
  std::string d_trig_tag_key;

  TimeControlPanel *d_controlpanel;
};

#endif /* TIME_DISPLAY_FORM_H */
