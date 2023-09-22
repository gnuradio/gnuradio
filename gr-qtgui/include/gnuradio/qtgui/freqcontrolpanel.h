/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef FREQ_CONTROL_PANEL_H
#define FREQ_CONTROL_PANEL_H

#include <gnuradio/fft/window.h>
#include <gnuradio/qtgui/displayform.h>
#include <gnuradio/qtgui/freqdisplayform.h>
#include <QtGui/QtGui>
#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <vector>

class FreqControlPanel : public QVBoxLayout
{
    Q_OBJECT

public:
    FreqControlPanel(FreqDisplayForm* form);
    ~FreqControlPanel() override;

public slots:
    void notifyAvgSlider(int val);
    void setFFTAverage(float val);
    void toggleGrid(bool en);
    void toggleAxisLabels(bool en);
    void toggleMaxHold(bool en);
    void toggleMinHold(bool en);

    void toggleFFTSize(int val);
    void toggleFFTWindow(const gr::fft::window::win_type win);

    void toggleTriggerMode(gr::qtgui::trigger_mode mode);

    void toggleStopButton();
    void updateStopLabel(bool on);

signals:
    void signalAvgSlider(float val);
    void signalAvg(bool en);
    void signalToggleStopButton();

private:
    FreqDisplayForm* d_parent;

    QGroupBox* d_trace_box;
    QVBoxLayout* d_trace_layout;
    QCheckBox* d_maxhold_check;
    QCheckBox* d_minhold_check;

    QHBoxLayout* d_avg_layout;
    QLabel* d_avg_label;
    QSlider* d_avg_slider;

    QGroupBox* d_axes_box;
    QVBoxLayout* d_axes_layout;

    QCheckBox* d_grid_check;
    QCheckBox* d_axislabels_check;
    QHBoxLayout* d_yrange_layout;
    QLabel* d_yrange_label;
    QPushButton* d_yrange_plus;
    QPushButton* d_yrange_minus;

    QHBoxLayout* d_ymin_layout;
    QLabel* d_ymin_label;
    QPushButton* d_ymin_plus;
    QPushButton* d_ymin_minus;

    QPushButton* d_autoscale_button;

    QGroupBox* d_fft_box;
    QVBoxLayout* d_fft_layout;
    QComboBox* d_fft_size_combo;
    QComboBox* d_fft_win_combo;

    int d_slider_max, d_slider_min, d_slider_step;

    QGroupBox* d_trigger_box;
    QVBoxLayout* d_trigger_layout;
    QHBoxLayout* d_trigger_level_layout;
    QLabel* d_trigger_level_label;
    QComboBox* d_trigger_mode_combo;
    QPushButton *d_trigger_level_plus, *d_trigger_level_minus;

    QGroupBox* d_extras_box;
    QVBoxLayout* d_extras_layout;
    QPushButton* d_stop_button;
};

#endif /* FREQ_CONTROL_PANEL_H */
