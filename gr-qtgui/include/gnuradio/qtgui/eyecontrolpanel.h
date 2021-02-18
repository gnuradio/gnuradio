/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef EYE_CONTROL_PANEL_H
#define EYE_CONTROL_PANEL_H

#include <gnuradio/qtgui/eyedisplayform.h>
#include <gnuradio/qtgui/eyedisplaysform.h>
#include <QtGui/QtGui>
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <vector>

class EyeControlPanel : public QVBoxLayout
{
    Q_OBJECT

public:
    EyeControlPanel(EyeDisplayForm* form);
    ~EyeControlPanel() override;

public slots:
    void toggleAutoScale(bool en);
    void toggleGrid(bool en);
    void toggleTriggerMode(gr::qtgui::trigger_mode mode);
    void toggleTriggerSlope(gr::qtgui::trigger_slope slope);
    void toggleStopButton();
    void updateStopLabel(bool on);

signals:
    void signalToggleStopButton();

private:
    EyeDisplayForm* d_parent;
    QGroupBox* d_axes_box;
    QGroupBox* d_trigger_box;
    QGroupBox* d_extras_box;

    QVBoxLayout* d_axes_layout;
    QHBoxLayout* d_yoff_layout;
    QHBoxLayout* d_yrange_layout;
    QVBoxLayout* d_trigger_layout;
    QHBoxLayout* d_trigger_level_layout;
    QHBoxLayout* d_trigger_delay_layout;
    QVBoxLayout* d_extras_layout;

    QLabel* d_yoff_label;
    QLabel* d_yrange_label;
    QLabel* d_trigger_level_label;
    QLabel* d_trigger_delay_label;

    QCheckBox* d_autoscale_check;
    QCheckBox* d_grid_check;
    QCheckBox* d_axislabels_check;

    QPushButton *d_yoff_plus, *d_yoff_minus;
    QPushButton *d_yrange_plus, *d_yrange_minus;
    QComboBox* d_trigger_mode_combo;
    QComboBox* d_trigger_slope_combo;
    QPushButton *d_trigger_level_plus, *d_trigger_level_minus;
    QPushButton *d_trigger_delay_plus, *d_trigger_delay_minus;
    QPushButton* d_autoscale_button;
    QPushButton* d_stop_button;
};

#endif /* EYE_CONTROL_PANEL_H */
