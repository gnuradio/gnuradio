/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/qtgui/displayform.h>

#include <QFileDialog>
#include <QPixmap>

DisplayForm::DisplayForm(int nplots, QWidget* parent)
    : QWidget(parent), d_nplots(nplots), d_system_specified_flag(false)
{
    d_axislabels = true;

    // Set the initial plot size
    resize(QSize(800, 600));

    // Set up a grid that can be turned on/off
    d_grid = new QwtPlotGrid();
    QPen* gridpen = new QPen(Qt::DashLine);
    gridpen->setWidthF(0.25);
    gridpen->setColor(Qt::gray);
    d_grid->setPen(*gridpen);

    // Create a set of actions for the menu
    d_stop_act = new QAction("Stop", this);
    d_stop_act->setStatusTip(tr("Start/Stop"));
    connect(d_stop_act, SIGNAL(triggered()), this, SLOT(setStop()));
    d_stop_state = false;

    d_grid_act = new QAction("Grid", this);
    d_grid_act->setCheckable(true);
    d_grid_act->setStatusTip(tr("Toggle Grid on/off"));
    connect(d_grid_act, SIGNAL(triggered(bool)), this, SLOT(setGrid(bool)));
    d_grid_state = false;

    d_axislabelsmenu = new QAction("Axis Labels", this);
    d_axislabelsmenu->setCheckable(true);
    d_axislabelsmenu->setStatusTip(tr("Toggle Axis Labels on/off"));
    connect(d_axislabelsmenu, SIGNAL(triggered(bool)), this, SLOT(setAxisLabels(bool)));

    // Create a pop-up menu for manipulating the figure
    d_menu_on = true;
    d_menu = new QMenu(this);
    d_menu->addAction(d_stop_act);
    d_menu->addAction(d_grid_act);
    d_menu->addAction(d_axislabelsmenu);

    for (unsigned int i = 0; i < d_nplots; ++i) {
        d_line_title_act.push_back(new LineTitleAction(i, this));
        d_line_color_menu.push_back(new LineColorMenu(i, this));
        d_line_width_menu.push_back(new LineWidthMenu(i, this));
        d_line_style_menu.push_back(new LineStyleMenu(i, this));
        d_line_marker_menu.push_back(new LineMarkerMenu(i, this));
        d_marker_alpha_menu.push_back(new MarkerAlphaMenu(i, this));

        connect(d_line_title_act[i],
                SIGNAL(whichTrigger(unsigned int, const QString&)),
                this,
                SLOT(setLineLabel(unsigned int, const QString&)));

        for (int j = 0; j < d_line_color_menu[i]->getNumActions(); j++) {
            connect(d_line_color_menu[i],
                    SIGNAL(whichTrigger(unsigned int, const QString&)),
                    this,
                    SLOT(setLineColor(unsigned int, const QString&)));
        }

        for (int j = 0; j < d_line_width_menu[i]->getNumActions(); j++) {
            connect(d_line_width_menu[i],
                    SIGNAL(whichTrigger(unsigned int, unsigned int)),
                    this,
                    SLOT(setLineWidth(unsigned int, unsigned int)));
        }

        for (int j = 0; j < d_line_style_menu[i]->getNumActions(); j++) {
            connect(d_line_style_menu[i],
                    SIGNAL(whichTrigger(unsigned int, Qt::PenStyle)),
                    this,
                    SLOT(setLineStyle(unsigned int, Qt::PenStyle)));
        }

        for (int j = 0; j < d_line_marker_menu[i]->getNumActions(); j++) {
            connect(d_line_marker_menu[i],
                    SIGNAL(whichTrigger(unsigned int, QwtSymbol::Style)),
                    this,
                    SLOT(setLineMarker(unsigned int, QwtSymbol::Style)));
        }

        for (int j = 0; j < d_marker_alpha_menu[i]->getNumActions(); j++) {
            connect(d_marker_alpha_menu[i],
                    SIGNAL(whichTrigger(unsigned int, unsigned int)),
                    this,
                    SLOT(setMarkerAlpha(unsigned int, unsigned int)));
        }

        d_lines_menu.push_back(new QMenu(tr(""), this));
        d_lines_menu[i]->addAction(d_line_title_act[i]);
        d_lines_menu[i]->addMenu(d_line_color_menu[i]);
        d_lines_menu[i]->addMenu(d_line_width_menu[i]);
        d_lines_menu[i]->addMenu(d_line_style_menu[i]);
        d_lines_menu[i]->addMenu(d_line_marker_menu[i]);
        d_lines_menu[i]->addMenu(d_marker_alpha_menu[i]);
        d_menu->addMenu(d_lines_menu[i]);
    }

    d_samp_rate_act = new PopupMenu("Sample Rate", this);
    d_samp_rate_act->setStatusTip(tr("Set Sample Rate"));
    connect(d_samp_rate_act,
            SIGNAL(whichTrigger(QString)),
            this,
            SLOT(setSampleRate(QString)));
    d_menu->addAction(d_samp_rate_act);

    d_autoscale_act = new QAction("Auto Scale", this);
    d_autoscale_act->setStatusTip(tr("Autoscale Plot"));
    d_autoscale_act->setCheckable(true);
    connect(d_autoscale_act, SIGNAL(triggered(bool)), this, SLOT(autoScale(bool)));
    d_autoscale_state = false;
    d_menu->addAction(d_autoscale_act);

    d_save_act = new QAction("Save", this);
    d_save_act->setStatusTip(tr("Save Figure"));
    connect(d_save_act, SIGNAL(triggered()), this, SLOT(saveFigure()));
    d_menu->addAction(d_save_act);

    Reset();
}

DisplayForm::~DisplayForm()
{
    // Qt deletes children when parent is deleted
    // Don't worry about deleting Display Plots - they are deleted when parents are
    // deleted
}

void DisplayForm::mousePressEvent(QMouseEvent* e)
{
    bool ctrloff = Qt::ControlModifier != QApplication::keyboardModifiers();
    if ((e->button() == Qt::MiddleButton) && ctrloff && (d_menu_on)) {
        if (d_stop_state == false)
            d_stop_act->setText(tr("Stop"));
        else
            d_stop_act->setText(tr("Start"));

        // Update the line titles if changed externally
        for (unsigned int i = 0; i < d_nplots; ++i) {
            d_lines_menu[i]->setTitle(d_display_plot->getLineLabel(i));
        }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)

        d_menu->exec(e->globalPos());
#else
        d_menu->exec(e->globalPosition().toPoint());
#endif
    }
}

void DisplayForm::updateGuiTimer() { d_display_plot->canvas()->update(); }

void DisplayForm::onPlotPointSelected(const QPointF p) { emit plotPointSelected(p, 3); }

void DisplayForm::Reset() {}

void DisplayForm::enableMenu(bool en) { d_menu_on = en; }

void DisplayForm::closeEvent(QCloseEvent* e)
{
    qApp->processEvents();
    QWidget::closeEvent(e);
}

void DisplayForm::setUpdateTime(double t) { d_update_time = t; }

void DisplayForm::setTitle(const QString& title) { d_display_plot->setTitle(title); }

void DisplayForm::setLineLabel(unsigned int which, const QString& label)
{
    d_display_plot->setLineLabel(which, label);
}

void DisplayForm::setLineColor(unsigned int which, const QString& color)
{
    QColor c = QColor(color);
    d_display_plot->setLineColor(which, c);
    d_display_plot->replot();
}

void DisplayForm::setLineWidth(unsigned int which, unsigned int width)
{
    d_display_plot->setLineWidth(which, width);
    d_display_plot->replot();
}

void DisplayForm::setLineStyle(unsigned int which, Qt::PenStyle style)
{
    d_display_plot->setLineStyle(which, style);
    d_display_plot->replot();
}

void DisplayForm::setLineMarker(unsigned int which, QwtSymbol::Style marker)
{
    d_display_plot->setLineMarker(which, marker);
    d_display_plot->replot();
}

void DisplayForm::setMarkerAlpha(unsigned int which, unsigned int alpha)
{
    d_display_plot->setMarkerAlpha(which, alpha);
    d_display_plot->replot();
}

QString DisplayForm::title() { return d_display_plot->title().text(); }

QString DisplayForm::lineLabel(unsigned int which)
{
    return d_display_plot->getLineLabel(which);
}

QString DisplayForm::lineColor(unsigned int which)
{
    return d_display_plot->getLineColor(which).name();
}

int DisplayForm::lineWidth(unsigned int which)
{
    return d_display_plot->getLineWidth(which);
}

Qt::PenStyle DisplayForm::lineStyle(unsigned int which)
{
    return d_display_plot->getLineStyle(which);
}

QwtSymbol::Style DisplayForm::lineMarker(unsigned int which)
{
    return d_display_plot->getLineMarker(which);
}

int DisplayForm::markerAlpha(unsigned int which)
{
    return d_display_plot->getMarkerAlpha(which);
}

void DisplayForm::setSampleRate(const QString& rate) {}

void DisplayForm::setStop(bool on)
{
    if (!on) {
        // will auto-detach if already attached.
        d_display_plot->setStop(false);
        d_stop_state = false;
    } else {
        d_display_plot->setStop(true);
        d_stop_state = true;
    }
    d_display_plot->replot();
}

void DisplayForm::setStop()
{
    if (d_stop_state == false)
        setStop(true);
    else
        setStop(false);
}

void DisplayForm::setGrid(bool on)
{
    if (on) {
        // will auto-detach if already attached.
        d_grid->attach(d_display_plot);
        d_grid_state = true;
    } else {
        d_grid->detach();
        d_grid_state = false;
    }
    d_grid_act->setChecked(on);
    d_display_plot->replot();
}

void DisplayForm::setAxisLabels(bool en)
{
    d_axislabels = en;
    d_axislabelsmenu->setChecked(en);
    getPlot()->setAxisLabels(d_axislabels);
}

void DisplayForm::saveFigure()
{
    QPixmap qpix = this->grab();

    QString types = QString(tr("JPEG file (*.jpg);;Portable Network Graphics file "
                               "(*.png);;Bitmap file (*.bmp);;TIFF file (*.tiff)"));

    QString filename, filetype;
    QFileDialog* filebox = new QFileDialog(0, "Save Image", "./", types);
    filebox->setViewMode(QFileDialog::Detail);
    filebox->setAcceptMode(QFileDialog::AcceptSave);
    filebox->setFileMode(QFileDialog::AnyFile);
    if (filebox->exec()) {
        filename = filebox->selectedFiles()[0];
        filetype = filebox->selectedNameFilter();
    } else {
        return;
    }

    if (filetype.contains(".jpg")) {
        qpix.save(filename + ".jpg", "JPEG");
    } else if (filetype.contains(".png")) {
        qpix.save(filename + ".png", "PNG");
    } else if (filetype.contains(".bmp")) {
        qpix.save(filename + ".bmp", "BMP");
    } else if (filetype.contains(".tiff")) {
        qpix.save(filename + ".tiff", "TIFF");
    } else {
        qpix.save(filename + ".jpg", "JPEG");
    }

    delete filebox;
}

void DisplayForm::disableLegend() { d_display_plot->disableLegend(); }
