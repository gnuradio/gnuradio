/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/qtgui/eyecontrolpanel.h>

EyeControlPanel::EyeControlPanel(EyeDisplayForm* form) : QVBoxLayout(), d_parent(form)
{
    // Set up the box for axes items
    d_axes_box = new QGroupBox("Axes");
    d_axes_layout = new QVBoxLayout;
    d_autoscale_check = new QCheckBox("Autoscale");
    d_grid_check = new QCheckBox("Grid");
    d_axislabels_check = new QCheckBox("Axis Labels");
    d_axislabels_check->setChecked(true);

    d_yoff_layout = new QHBoxLayout;
    d_yoff_label = new QLabel("Y Offset:");
    d_yoff_plus = new QPushButton("+");
    d_yoff_minus = new QPushButton("-");
    d_yoff_plus->setMaximumWidth(30);
    d_yoff_minus->setMaximumWidth(30);
    d_yoff_layout->addWidget(d_yoff_label);
    d_yoff_layout->addWidget(d_yoff_plus);
    d_yoff_layout->addWidget(d_yoff_minus);

    d_yrange_layout = new QHBoxLayout;
    d_yrange_label = new QLabel("Y Range:");
    d_yrange_plus = new QPushButton("+");
    d_yrange_minus = new QPushButton("-");
    d_yrange_plus->setMaximumWidth(30);
    d_yrange_minus->setMaximumWidth(30);
    d_yrange_layout->addWidget(d_yrange_label);
    d_yrange_layout->addWidget(d_yrange_plus);
    d_yrange_layout->addWidget(d_yrange_minus);

    // Set up the box for trigger items
    d_trigger_box = new QGroupBox("Trigger");
    d_trigger_layout = new QVBoxLayout;
    d_trigger_mode_combo = new QComboBox();
    d_trigger_mode_combo->addItem("Free");
    d_trigger_mode_combo->addItem("Auto");
    d_trigger_mode_combo->addItem("Normal");
    d_trigger_mode_combo->addItem("Tag");

    d_trigger_slope_combo = new QComboBox();
    d_trigger_slope_combo->addItem("Positive");
    d_trigger_slope_combo->addItem("Negative");

    d_trigger_level_layout = new QHBoxLayout;
    d_trigger_level_label = new QLabel("Level:");
    d_trigger_level_plus = new QPushButton("+");
    d_trigger_level_minus = new QPushButton("-");
    d_trigger_level_plus->setMaximumWidth(30);
    d_trigger_level_minus->setMaximumWidth(30);
    d_trigger_level_layout->addWidget(d_trigger_level_label);
    d_trigger_level_layout->addWidget(d_trigger_level_plus);
    d_trigger_level_layout->addWidget(d_trigger_level_minus);

    d_trigger_delay_layout = new QHBoxLayout;
    d_trigger_delay_label = new QLabel("Delay:");
    d_trigger_delay_plus = new QPushButton("+");
    d_trigger_delay_minus = new QPushButton("-");
    d_trigger_delay_plus->setMaximumWidth(30);
    d_trigger_delay_minus->setMaximumWidth(30);
    d_trigger_delay_layout->addWidget(d_trigger_delay_label);
    d_trigger_delay_layout->addWidget(d_trigger_delay_plus);
    d_trigger_delay_layout->addWidget(d_trigger_delay_minus);

    // Set up the box for other items
    d_extras_box = new QGroupBox("Extras");
    d_extras_layout = new QVBoxLayout;
    d_autoscale_button = new QPushButton("Autoscale");
    d_stop_button = new QPushButton(
        QApplication::style()->standardIcon(QStyle::SP_MediaStop), "Stop");
    d_stop_button->setCheckable(true);

    // Set up the boxes into the layout
    d_axes_layout->addWidget(d_autoscale_check);
    d_axes_layout->addWidget(d_grid_check);
    d_axes_layout->addWidget(d_axislabels_check);
    d_axes_layout->addLayout(d_yoff_layout);
    d_axes_layout->addLayout(d_yrange_layout);
    d_axes_box->setLayout(d_axes_layout);

    d_trigger_layout->addWidget(d_trigger_mode_combo);
    d_trigger_layout->addWidget(d_trigger_slope_combo);
    d_trigger_layout->addLayout(d_trigger_level_layout);
    d_trigger_layout->addLayout(d_trigger_delay_layout);
    d_trigger_box->setLayout(d_trigger_layout);

    d_extras_layout->addWidget(d_autoscale_button);
    d_extras_layout->addWidget(d_stop_button);
    d_extras_box->setLayout(d_extras_layout);

    addWidget(d_axes_box);
    addWidget(d_trigger_box);
    addWidget(d_extras_box);
    addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Connect up the control signals/slots
    connect(d_autoscale_check, SIGNAL(clicked(bool)), d_parent, SLOT(autoScale(bool)));
    connect(d_grid_check, SIGNAL(clicked(bool)), d_parent, SLOT(setGrid(bool)));
    connect(
        d_axislabels_check, SIGNAL(clicked(bool)), d_parent, SLOT(setAxisLabels(bool)));
    connect(d_yoff_plus, SIGNAL(pressed(void)), d_parent, SLOT(notifyYAxisPlus(void)));
    connect(d_yoff_minus, SIGNAL(pressed(void)), d_parent, SLOT(notifyYAxisMinus(void)));

    connect(d_yrange_plus, SIGNAL(pressed(void)), d_parent, SLOT(notifyYRangePlus(void)));
    connect(
        d_yrange_minus, SIGNAL(pressed(void)), d_parent, SLOT(notifyYRangeMinus(void)));

    connect(d_trigger_mode_combo,
            SIGNAL(currentIndexChanged(const QString&)),
            d_parent,
            SLOT(notifyTriggerMode(const QString&)));
    connect(d_trigger_slope_combo,
            SIGNAL(currentIndexChanged(const QString&)),
            d_parent,
            SLOT(notifyTriggerSlope(const QString&)));
    connect(d_trigger_level_plus,
            SIGNAL(pressed(void)),
            d_parent,
            SLOT(notifyTriggerLevelPlus()));
    connect(d_trigger_level_minus,
            SIGNAL(pressed(void)),
            d_parent,
            SLOT(notifyTriggerLevelMinus()));
    connect(d_trigger_delay_plus,
            SIGNAL(pressed(void)),
            d_parent,
            SLOT(notifyTriggerDelayPlus()));
    connect(d_trigger_delay_minus,
            SIGNAL(pressed(void)),
            d_parent,
            SLOT(notifyTriggerDelayMinus()));

    connect(
        d_autoscale_button, SIGNAL(pressed(void)), d_parent, SLOT(autoScaleShot(void)));

    // Handle the start/stop button
    // Call the base class' stop function when they press the button
    connect(d_stop_button, SIGNAL(pressed(void)), d_parent, SLOT(setStop(void)));
    // Updated the button state regardless of who changed it
    connect(d_stop_button, SIGNAL(toggled(bool)), this, SLOT(updateStopLabel(bool)));
    connect(
        this, SIGNAL(signalToggleStopButton(void)), d_stop_button, SLOT(toggle(void)));
}

EyeControlPanel::~EyeControlPanel()
{
    removeWidget(d_axes_box);
    removeWidget(d_trigger_box);
    removeWidget(d_extras_box);
    delete d_axes_box;
    delete d_trigger_box;
    delete d_extras_box;

    // All other children of the boxes are automatically deleted.
}

void EyeControlPanel::toggleAutoScale(bool en) { d_autoscale_check->setChecked(en); }

void EyeControlPanel::toggleGrid(bool en) { d_grid_check->setChecked(en); }

void EyeControlPanel::toggleTriggerMode(gr::qtgui::trigger_mode mode)
{
    d_trigger_mode_combo->setCurrentIndex(static_cast<int>(mode));
}

void EyeControlPanel::toggleTriggerSlope(gr::qtgui::trigger_slope slope)
{
    d_trigger_slope_combo->setCurrentIndex(static_cast<int>(slope));
}

void EyeControlPanel::toggleStopButton() { emit signalToggleStopButton(); }

void EyeControlPanel::updateStopLabel(bool on)
{
    if (on) {
        d_stop_button->setText("Start");
        d_stop_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    } else {
        d_stop_button->setText("Stop");
        d_stop_button->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop));
    }
}
