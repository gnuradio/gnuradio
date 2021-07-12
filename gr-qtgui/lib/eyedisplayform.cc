/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/qtgui/eyecontrolpanel.h>

#include <gnuradio/qtgui/eyedisplayform.h>

#include <QGroupBox>
#include <QMessageBox>
#include <QSpacerItem>

#include <cmath>

EyeDisplayForm::EyeDisplayForm(int nplots, bool cmplx, QWidget* parent)
    : EyeDisplaysForm(nplots, parent)
{
    d_current_units = 1;

    d_trig_mode = gr::qtgui::TRIG_MODE_FREE;
    d_trig_slope = gr::qtgui::TRIG_SLOPE_POS;
    d_trig_level = 0;
    d_trig_delay = 0;
    d_trig_channel = 0;
    d_trig_tag_key = "";

    d_int_validator = new QIntValidator(this);
    d_int_validator->setBottom(0);

    d_layout = new QGridLayout(this);
    d_controlpanel = NULL;

    unsigned int i = 0;
    if ((cmplx == true) || ((cmplx == false) && (d_nplots > 3))) {
        // Split eye patterns on 2 columns
        d_rows = std::floor((d_nplots + 1) / 2);
        d_cols = 2;
    } else {
        d_rows = d_nplots;
        d_cols = 1;
    }

    // Setup the layout of the display
    for (unsigned int row = 0; row < d_rows; ++row) {
        for (unsigned int col = 0; col < d_cols; ++col) {
            if (i < d_nplots) {
                d_displays_plot.push_back(new EyeDisplayPlot(d_nplots, i, this));
                d_layout->addWidget(d_displays_plot[i], row, col);
                d_display_plot = d_displays_plot[i];
                ++i;
            }
        }
    }

    for (unsigned int row = 0; row < d_rows; ++row) {
        d_layout->setRowStretch(row, 1);
    }

    for (unsigned int col = 0; col < d_cols; ++col) {
        d_layout->setColumnStretch(col, 1);
    }
    setLayout(d_layout);

    d_nptsmenu = new NPointsMenu(this);
    d_menu->addAction(d_nptsmenu);
    connect(d_nptsmenu, SIGNAL(whichTrigger(int)), this, SLOT(setNPoints(const int)));
    connect(
        this, SIGNAL(signalNPoints(const int)), d_nptsmenu, SLOT(setDiagText(const int)));

    for (unsigned int i = 0; i < d_nplots; ++i) {
        d_tagsmenu.push_back(new QAction("Show Tag Makers", this));
        d_tagsmenu[i]->setCheckable(true);
        d_tagsmenu[i]->setChecked(true);
        connect(d_tagsmenu[i], SIGNAL(triggered(bool)), this, SLOT(tagMenuSlot(bool)));
        d_lines_menu[i]->addAction(d_tagsmenu[i]);
    }

    // Set up the trigger menu
    d_triggermenu = new QMenu("Trigger", this);
    d_tr_mode_menu = new TriggerModeMenu(this);
    d_tr_slope_menu = new TriggerSlopeMenu(this);
    d_tr_level_act = new PopupMenu("Level", this);
    d_tr_delay_act = new PopupMenu("Delay", this);
    d_tr_channel_menu = new TriggerChannelMenu(nplots, this);
    d_tr_tag_key_act = new PopupMenu("Tag Key", this);
    d_triggermenu->addMenu(d_tr_mode_menu);
    d_triggermenu->addMenu(d_tr_slope_menu);
    d_triggermenu->addAction(d_tr_level_act);
    d_triggermenu->addAction(d_tr_delay_act);
    d_triggermenu->addMenu(d_tr_channel_menu);
    d_triggermenu->addAction(d_tr_tag_key_act);
    d_menu->addMenu(d_triggermenu);

    d_controlpanelmenu = new QAction("Control Panel", this);
    d_controlpanelmenu->setCheckable(true);
    d_menu->addAction(d_controlpanelmenu);
    connect(
        d_controlpanelmenu, SIGNAL(triggered(bool)), this, SLOT(setupControlPanel(bool)));

    setTriggerMode(gr::qtgui::TRIG_MODE_FREE);
    setTriggerSlope(gr::qtgui::TRIG_SLOPE_POS);

    connect(d_tr_mode_menu,
            SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
            this,
            SLOT(setTriggerMode(gr::qtgui::trigger_mode)));
    // updates trigger state by calling set level or set tag key.
    connect(d_tr_mode_menu,
            SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
            this,
            SLOT(updateTrigger(gr::qtgui::trigger_mode)));

    connect(d_tr_slope_menu,
            SIGNAL(whichTrigger(gr::qtgui::trigger_slope)),
            this,
            SLOT(setTriggerSlope(gr::qtgui::trigger_slope)));

    setTriggerLevel(0);
    connect(d_tr_level_act,
            SIGNAL(whichTrigger(QString)),
            this,
            SLOT(setTriggerLevel(QString)));
    connect(this, SIGNAL(signalTriggerLevel(float)), this, SLOT(setTriggerLevel(float)));

    setTriggerDelay(0);
    connect(d_tr_delay_act,
            SIGNAL(whichTrigger(QString)),
            this,
            SLOT(setTriggerDelay(QString)));
    connect(this, SIGNAL(signalTriggerDelay(float)), this, SLOT(setTriggerDelay(float)));

    setTriggerChannel(0);
    connect(
        d_tr_channel_menu, SIGNAL(whichTrigger(int)), this, SLOT(setTriggerChannel(int)));

    setTriggerTagKey(std::string(""));
    connect(d_tr_tag_key_act,
            SIGNAL(whichTrigger(QString)),
            this,
            SLOT(setTriggerTagKey(QString)));

    Reset();

    connect(d_display_plot,
            SIGNAL(plotPointSelected(const QPointF)),
            this,
            SLOT(onPlotPointSelected(const QPointF)));

    for (unsigned int i = 0; i < d_nplots; ++i) {
        connect(this, SIGNAL(signalReplot()), getSinglePlot(i), SLOT(replot()));
    }
}

EyeDisplayForm::~EyeDisplayForm()
{
    // Qt deletes children when parent is deleted

    // Don't worry about deleting Display Plots - they are deleted when parents are
    // deleted
    delete d_int_validator;

    teardownControlPanel();
}

void EyeDisplayForm::setupControlPanel(bool en)
{
    if (en) {
        setupControlPanel();
    } else {
        teardownControlPanel();
    }
}

void EyeDisplayForm::setupControlPanel()
{
    if (d_controlpanel)
        delete d_controlpanel;

    // Create the control panel layout
    d_controlpanel = new EyeControlPanel(this);

    // Connect action items in menu to controlpanel widgets
    connect(d_autoscale_act,
            SIGNAL(triggered(bool)),
            d_controlpanel,
            SLOT(toggleAutoScale(bool)));
    connect(d_grid_act, SIGNAL(triggered(bool)), d_controlpanel, SLOT(toggleGrid(bool)));
    connect(d_tr_mode_menu,
            SIGNAL(whichTrigger(gr::qtgui::trigger_mode)),
            d_controlpanel,
            SLOT(toggleTriggerMode(gr::qtgui::trigger_mode)));
    connect(this,
            SIGNAL(signalTriggerMode(gr::qtgui::trigger_mode)),
            d_controlpanel,
            SLOT(toggleTriggerMode(gr::qtgui::trigger_mode)));
    connect(d_tr_slope_menu,
            SIGNAL(whichTrigger(gr::qtgui::trigger_slope)),
            d_controlpanel,
            SLOT(toggleTriggerSlope(gr::qtgui::trigger_slope)));
    connect(this,
            SIGNAL(signalTriggerSlope(gr::qtgui::trigger_slope)),
            d_controlpanel,
            SLOT(toggleTriggerSlope(gr::qtgui::trigger_slope)));
    connect(d_stop_act, SIGNAL(triggered()), d_controlpanel, SLOT(toggleStopButton()));

    d_layout->addLayout(d_controlpanel, 0, d_cols, d_rows, 1);

    d_controlpanel->toggleAutoScale(d_autoscale_act->isChecked());
    d_controlpanel->toggleGrid(d_grid_act->isChecked());
    d_controlpanel->toggleTriggerMode(getTriggerMode());
    d_controlpanel->toggleTriggerSlope(getTriggerSlope());

    d_controlpanelmenu->setChecked(true);
}

void EyeDisplayForm::teardownControlPanel()
{
    if (d_controlpanel) {
        d_layout->removeItem(d_controlpanel);
        delete d_controlpanel;
        d_controlpanel = NULL;
    }
    d_controlpanelmenu->setChecked(false);
}

EyeDisplayPlot* EyeDisplayForm::getSinglePlot(unsigned int i)
{
    return ((EyeDisplayPlot*)d_displays_plot[i]);
}

void EyeDisplayForm::newData(const QEvent* updateEvent)
{
    TimeUpdateEvent* tevent = (TimeUpdateEvent*)updateEvent;
    const std::vector<double*> dataPoints = tevent->getTimeDomainPoints();
    const uint64_t numDataPoints = tevent->getNumTimeDomainDataPoints();
    const std::vector<std::vector<gr::tag_t>> tags = tevent->getTags();

    for (unsigned int i = 0; i < d_nplots; ++i) {
        getSinglePlot(i)->plotNewData(
            dataPoints, numDataPoints, d_sps, d_update_time, tags);
    }
}

void EyeDisplayForm::customEvent(QEvent* e)
{
    if (e->type() == TimeUpdateEvent::Type()) {
        newData(e);
    }
}

void EyeDisplayForm::setSampleRate(const QString& samprate)
{
    setSampleRate(samprate.toDouble());
}

void EyeDisplayForm::setSampleRate(const double samprate)
{
    if (samprate > 0) {
        std::string strtime[4] = { "sec", "ms", "us", "ns" };
        double units10 = floor(log10(samprate));
        double units3 = std::max(floor(units10 / 3.0), 0.0);
        double units = pow(10, (units10 - fmod(units10, 3.0)));
        int iunit = static_cast<int>(units3);

        d_current_units = units;

        for (unsigned int i = 0; i < d_nplots; ++i) {
            getSinglePlot(i)->setSampleRate(samprate, units, strtime[iunit]);
        }
    } else {
        throw std::runtime_error("EyeDisplayForm: samprate must be > 0.");
    }
}

void EyeDisplayForm::setYaxis(double min, double max)
{
    for (unsigned int i = 0; i < d_nplots; ++i) {
        getSinglePlot(i)->setYaxis(min, max);
    }
}

void EyeDisplayForm::setAxisLabels(bool en)
{

    for (unsigned int i = 0; i < d_nplots; ++i) {
        getSinglePlot(i)->setAxisLabels(en);
    }
}

void EyeDisplayForm::setYLabel(const std::string& label, const std::string& unit)
{
    for (unsigned int i = 0; i < d_nplots; ++i) {
        getSinglePlot(i)->setYLabel(label, unit);
    }
}

int EyeDisplayForm::getNPoints() const { return d_npoints; }

int EyeDisplayForm::getSamplesPerSymbol() const { return d_sps; }

void EyeDisplayForm::setNPoints(const int npoints)
{
    d_npoints = npoints;
    emit signalNPoints(npoints);
}

void EyeDisplayForm::autoScale(bool en)
{
    d_autoscale_state = en;
    d_autoscale_act->setChecked(en);
    for (unsigned int i = 0; i < d_nplots; ++i) {
        getSinglePlot(i)->setAutoScale(d_autoscale_state);
    }
    emit signalReplot();
}

void EyeDisplayForm::autoScaleShot()
{
    for (unsigned int i = 0; i < d_nplots; ++i) {
        getSinglePlot(i)->setAutoScaleShot();
    }
    emit signalReplot();
}

void EyeDisplayForm::setTagMenu(unsigned int which, bool en)
{
    getSinglePlot(which)->enableTagMarker(0, en);
    d_tagsmenu[which]->setChecked(en);
}

void EyeDisplayForm::tagMenuSlot(bool en)
{
    for (size_t i = 0; i < d_tagsmenu.size(); i++) {
        getSinglePlot(i)->enableTagMarker(0, d_tagsmenu[i]->isChecked());
    }
}

/********************************************************************
 * TRIGGER METHODS
 *******************************************************************/

void EyeDisplayForm::setTriggerMode(gr::qtgui::trigger_mode mode)
{
    d_trig_mode = mode;
    d_tr_mode_menu->getAction(mode)->setChecked(true);

    if ((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) ||
        (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
        getSinglePlot(d_trig_channel)->attachTriggerLines(true);
    } else {
        getSinglePlot(d_trig_channel)->attachTriggerLines(false);
    }

    emit signalReplot();
    emit signalTriggerMode(mode);
}

void EyeDisplayForm::updateTrigger(gr::qtgui::trigger_mode mode)
{
    // If auto or normal mode, popup trigger level box to set
    if ((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) ||
        (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
        d_tr_level_act->activate(QAction::Trigger);
        getSinglePlot(d_trig_channel)->attachTriggerLines(true);
    } else {
        getSinglePlot(d_trig_channel)->attachTriggerLines(false);
    }

    // if tag mode, popup tag key box to set
    if ((d_trig_tag_key.empty()) && (d_trig_mode == gr::qtgui::TRIG_MODE_TAG))
        d_tr_tag_key_act->activate(QAction::Trigger);

    emit signalReplot();
    emit signalTriggerMode(mode);
}

gr::qtgui::trigger_mode EyeDisplayForm::getTriggerMode() const { return d_trig_mode; }

void EyeDisplayForm::setTriggerSlope(gr::qtgui::trigger_slope slope)
{
    d_trig_slope = slope;
    d_tr_slope_menu->getAction(slope)->setChecked(true);

    emit signalReplot();
    emit signalTriggerSlope(slope);
}

gr::qtgui::trigger_slope EyeDisplayForm::getTriggerSlope() const { return d_trig_slope; }

void EyeDisplayForm::setTriggerLevel(QString s)
{
    d_trig_level = s.toFloat();

    if ((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) ||
        (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
        getSinglePlot(d_trig_channel)
            ->setTriggerLines(d_trig_delay * d_current_units, d_trig_level);
    }

    emit signalReplot();
}

void EyeDisplayForm::setTriggerLevel(float level)
{
    d_trig_level = level;
    d_tr_level_act->setText(QString().setNum(d_trig_level));

    if ((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) ||
        (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
        getSinglePlot(d_trig_channel)
            ->setTriggerLines(d_trig_delay * d_current_units, d_trig_level);
    }

    emit signalReplot();
}

float EyeDisplayForm::getTriggerLevel() const { return d_trig_level; }

void EyeDisplayForm::setTriggerDelay(QString s)
{
    d_trig_delay = s.toFloat();

    if ((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) ||
        (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
        getSinglePlot(d_trig_channel)
            ->setTriggerLines(d_trig_delay * d_current_units, d_trig_level);
    }

    emit signalReplot();
}

void EyeDisplayForm::setTriggerDelay(float delay)
{
    d_trig_delay = delay;
    d_tr_delay_act->setText(QString().setNum(d_trig_delay));

    if ((d_trig_mode == gr::qtgui::TRIG_MODE_AUTO) ||
        (d_trig_mode == gr::qtgui::TRIG_MODE_NORM)) {
        getSinglePlot(d_trig_channel)
            ->setTriggerLines(d_trig_delay * d_current_units, d_trig_level);
    }

    emit signalReplot();
}

float EyeDisplayForm::getTriggerDelay() const { return d_trig_delay; }

void EyeDisplayForm::setTriggerChannel(int channel)
{
    getSinglePlot(d_trig_channel)->attachTriggerLines(false);
    d_trig_channel = channel;
    updateTrigger(d_trig_mode);
    d_tr_channel_menu->getAction(d_trig_channel)->setChecked(true);

    emit signalReplot();
}

int EyeDisplayForm::getTriggerChannel() const { return d_trig_channel; }

void EyeDisplayForm::setTriggerTagKey(QString s)
{
    d_trig_tag_key = s.toStdString();

    emit signalReplot();
}

void EyeDisplayForm::setTriggerTagKey(const std::string& key)
{
    d_trig_tag_key = key;
    d_tr_tag_key_act->setText(QString().fromStdString(d_trig_tag_key));

    emit signalReplot();
}

std::string EyeDisplayForm::getTriggerTagKey() const { return d_trig_tag_key; }

/********************************************************************
 * Notification messages from the control panel
 *******************************************************************/

void EyeDisplayForm::notifyYAxisPlus()
{
    for (unsigned int i = 0; i < d_nplots; ++i) {

#if QWT_VERSION < 0x060100
        QwtScaleDiv* ax = getSinglePlot(i)->axisScaleDiv(QwtPlot::yLeft);
        double range = ax->upperBound() - ax->lowerBound();
        double step = range / 20.0;
        getSinglePlot(i)->setYaxis(ax->lowerBound() + step, ax->upperBound() + step);

#else

        QwtScaleDiv ax = getSinglePlot(i)->axisScaleDiv(QwtPlot::yLeft);
        double range = ax.upperBound() - ax.lowerBound();
        double step = range / 20.0;
        getSinglePlot(i)->setYaxis(ax.lowerBound() + step, ax.upperBound() + step);
#endif
    }
}

void EyeDisplayForm::notifyYAxisMinus()
{
    for (unsigned int i = 0; i < d_nplots; ++i) {

#if QWT_VERSION < 0x060100
        QwtScaleDiv* ax = getSinglePlot(i)->axisScaleDiv(QwtPlot::yLeft);
        double range = ax->upperBound() - ax->lowerBound();
        double step = range / 20.0;
        getSinglePlot(i)->setYaxis(ax->lowerBound() - step, ax->upperBound() - step);

#else

        QwtScaleDiv ax = getSinglePlot(i)->axisScaleDiv(QwtPlot::yLeft);
        double range = ax.upperBound() - ax.lowerBound();
        double step = range / 20.0;
        getSinglePlot(i)->setYaxis(ax.lowerBound() - step, ax.upperBound() - step);
#endif
    }
}

void EyeDisplayForm::notifyYRangePlus()
{
    for (unsigned int i = 0; i < d_nplots; ++i) {

#if QWT_VERSION < 0x060100
        QwtScaleDiv* ax = getSinglePlot(i)->axisScaleDiv(QwtPlot::yLeft);
        double range = ax->upperBound() - ax->lowerBound();
        double step = range / 20.0;
        getSinglePlot(i)->setYaxis(ax->lowerBound() - step, ax->upperBound() + step);

#else

        QwtScaleDiv ax = getSinglePlot(i)->axisScaleDiv(QwtPlot::yLeft);
        double range = ax.upperBound() - ax.lowerBound();
        double step = range / 20.0;
        getSinglePlot(i)->setYaxis(ax.lowerBound() - step, ax.upperBound() + step);
#endif
    }
}

void EyeDisplayForm::notifyYRangeMinus()
{
    for (unsigned int i = 0; i < d_nplots; ++i) {

#if QWT_VERSION < 0x060100
        QwtScaleDiv* ax = getSinglePlot(i)->axisScaleDiv(QwtPlot::yLeft);
        double range = ax->upperBound() - ax->lowerBound();
        double step = range / 20.0;
        getSinglePlot(i)->setYaxis(ax->lowerBound() + step, ax->upperBound() - step);

#else

        QwtScaleDiv ax = getSinglePlot(i)->axisScaleDiv(QwtPlot::yLeft);
        double range = ax.upperBound() - ax.lowerBound();
        double step = range / 20.0;
        getSinglePlot(i)->setYaxis(ax.lowerBound() + step, ax.upperBound() - step);
#endif
    }
}

void EyeDisplayForm::notifyTriggerMode(const QString& mode)
{
    if (mode == "Free") {
        setTriggerMode(gr::qtgui::TRIG_MODE_FREE);
    } else if (mode == "Auto") {
        setTriggerMode(gr::qtgui::TRIG_MODE_AUTO);
    } else if (mode == "Normal") {
        setTriggerMode(gr::qtgui::TRIG_MODE_NORM);
    } else if (mode == "Tag") {
        setTriggerMode(gr::qtgui::TRIG_MODE_TAG);
        updateTrigger(gr::qtgui::TRIG_MODE_TAG);
    }
}

void EyeDisplayForm::notifyTriggerSlope(const QString& slope)
{
    if (slope == "Positive") {
        setTriggerSlope(gr::qtgui::TRIG_SLOPE_POS);
    } else if (slope == "Negative") {
        setTriggerSlope(gr::qtgui::TRIG_SLOPE_NEG);
    }
}

void EyeDisplayForm::notifyTriggerLevelPlus()
{
#if QWT_VERSION < 0x060100
    QwtScaleDiv* ax = getSinglePlot(0)->axisScaleDiv(QwtPlot::yLeft);
    double range = ax->upperBound() - ax->lowerBound();

#else

    QwtScaleDiv ax = getSinglePlot(0)->axisScaleDiv(QwtPlot::yLeft);
    double range = ax.upperBound() - ax.lowerBound();
#endif

    double step = range / 20.0;
    emit signalTriggerLevel(getTriggerLevel() + step);
}

void EyeDisplayForm::notifyTriggerLevelMinus()
{
#if QWT_VERSION < 0x060100
    QwtScaleDiv* ax = getSinglePlot(0)->axisScaleDiv(QwtPlot::yLeft);
    double range = ax->upperBound() - ax->lowerBound();

#else

    QwtScaleDiv ax = getSinglePlot(0)->axisScaleDiv(QwtPlot::yLeft);
    double range = ax.upperBound() - ax.lowerBound();
#endif

    double step = range / 20.0;
    emit signalTriggerLevel(getTriggerLevel() - step);
}

void EyeDisplayForm::notifyTriggerDelayPlus()
{
#if QWT_VERSION < 0x060100
    QwtScaleDiv* ax = getSinglePlot(0)->axisScaleDiv(QwtPlot::xBottom);
    double range = ax->upperBound() - ax->lowerBound();

#else

    QwtScaleDiv ax = getSinglePlot(0)->axisScaleDiv(QwtPlot::xBottom);
    double range = ax.upperBound() - ax.lowerBound();
#endif

    double step = range / (2 * d_sps);
    double trig = getTriggerDelay() + step / d_current_units;

    if (trig > range / d_current_units)
        trig = range / d_current_units;
    emit signalTriggerDelay(trig);
}

void EyeDisplayForm::notifyTriggerDelayMinus()
{
#if QWT_VERSION < 0x060100
    QwtScaleDiv* ax = getSinglePlot(0)->axisScaleDiv(QwtPlot::xBottom);
    double range = ax->upperBound() - ax->lowerBound();

#else

    QwtScaleDiv ax = getSinglePlot(0)->axisScaleDiv(QwtPlot::xBottom);
    double range = ax.upperBound() - ax.lowerBound();
#endif

    double step = range / (2 * d_sps);
    double trig = getTriggerDelay() - step / d_current_units;

    if (trig < 0)
        trig = 0;
    emit signalTriggerDelay(trig);
}
