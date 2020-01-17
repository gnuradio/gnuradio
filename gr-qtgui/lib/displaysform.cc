/* -*- c++ -*- */
/*
 * Copyright 2012,2020 Free Software Foundation, Inc.
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

#include <gnuradio/qtgui/displaysform.h>

#include <QFileDialog>
#include <QPixmap>
#include <iostream>

DisplaysForm::DisplaysForm(int nplots, QWidget* parent) :
		QWidget(parent), d_nplots(nplots), d_system_specified_flag(false) {
	d_isclosed = false;
	d_axislabels = true;

	// Set the initial plot size
	resize(QSize(800, 600));

	// Set up a grid that can be turned on/off
	QPen* gridpen = new QPen(Qt::DashLine);
	gridpen->setWidthF(1.0);
	gridpen->setColor(Qt::gray);

	// Each eye must have one grid attached
	for (unsigned int i = 0; i < d_nplots; ++i) {
		d_grids.push_back(new QwtPlotGrid());
		d_grids[i]->setPen(*gridpen);
	}

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
connect(d_samp_rate_act, SIGNAL(whichTrigger(QString)), this,
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

DisplaysForm::~DisplaysForm() {
d_isclosed = true;

 // Qt deletes children when parent is deleted
 // Don't worry about deleting Display Plots - they are deleted when parents are
 // deleted
}

void DisplaysForm::resizeEvent(QResizeEvent* e) {
    // QSize s = size();
	// emit d_display_plot->resizeSlot(&s);
}

void DisplaysForm::mousePressEvent(QMouseEvent* e) {
bool ctrloff = Qt::ControlModifier != QApplication::keyboardModifiers();
if ((e->button() == Qt::MidButton) && ctrloff && (d_menu_on)) {
if (d_stop_state == false)
d_stop_act->setText(tr("Stop"));
else
d_stop_act->setText(tr("Start"));

    // Update the line titles if changed externally
for (unsigned int i = 0; i < d_nplots; ++i) {
d_lines_menu[i]->setTitle(d_displays_plot[i]->title().text());
}
d_menu->exec(e->globalPos());
}
}

void DisplaysForm::updateGuiTimer() {
d_display_plot->canvas()->update();
}

void DisplaysForm::onPlotPointSelected(const QPointF p) {
emit plotPointSelected(p, 3);
}

void DisplaysForm::Reset() {
}

bool DisplaysForm::isClosed() const {
return d_isclosed;
}

void DisplaysForm::enableMenu(bool en) {
d_menu_on = en;
}

void DisplaysForm::closeEvent(QCloseEvent* e) {
d_isclosed = true;
qApp->processEvents();
QWidget::closeEvent(e);
}

void DisplaysForm::setUpdateTime(double t) {
d_update_time = t;
}

void DisplaysForm::setSamplesPerSymbol(int64_t samples_per_symbol) {
d_sps = (int) samples_per_symbol;
}

void DisplaysForm::setTitle(const QString& title) { /* Unused but do not remove */
}

void DisplaysForm::setLineLabel(unsigned int which, const QString& label) {
    // Line label used as eye pattern title
d_displays_plot[which]->setTitle(label);
}

void DisplaysForm::setLineColor(unsigned int which, const QString& color) {
QColor c = QColor(color);
d_displays_plot[which]->setLineColor(0, c);
d_displays_plot[which]->replot();
}

void DisplaysForm::setLineWidth(unsigned int which, unsigned int width) {
d_displays_plot[which]->setLineWidth(0, width);
d_displays_plot[which]->replot();
}

void DisplaysForm::setLineStyle(unsigned int which, Qt::PenStyle style) {
d_displays_plot[which]->setLineStyle(0, style);
d_displays_plot[which]->replot();
}

void DisplaysForm::setLineMarker(unsigned int which, QwtSymbol::Style marker) {
d_displays_plot[which]->setLineMarker(0, marker);
d_displays_plot[which]->replot();
}

void DisplaysForm::setMarkerAlpha(unsigned int which, unsigned int alpha) {
d_displays_plot[which]->setMarkerAlpha(0, alpha);
d_displays_plot[which]->replot();
}

QString DisplaysForm::title() { /* Unused */
return "";
}

QString DisplaysForm::lineLabel(unsigned int which) {
return d_displays_plot[which]->title().text();
}

QString DisplaysForm::lineColor(unsigned int which) {
return d_displays_plot[which]->getLineColor(0).name();
}

int DisplaysForm::lineWidth(unsigned int which) {
return d_displays_plot[which]->getLineWidth(0);
}

Qt::PenStyle DisplaysForm::lineStyle(unsigned int which) {
return d_displays_plot[which]->getLineStyle(0);
}

QwtSymbol::Style DisplaysForm::lineMarker(unsigned int which) {
return d_displays_plot[which]->getLineMarker(0);
}

int DisplaysForm::markerAlpha(unsigned int which) {
return d_displays_plot[which]->getMarkerAlpha(0);
}

void DisplaysForm::setSampleRate(const QString& rate) {
}

void DisplaysForm::setStop(bool on) {
for (unsigned int i = 0; i < d_nplots; ++i) {
if (!on) {
    // will auto-detach if already attached.
d_displays_plot[i]->setStop(false);
d_stop_state = false;
} else {
d_displays_plot[i]->setStop(true);
d_stop_state = true;
}
d_displays_plot[i]->replot();
}
}

void DisplaysForm::setStop() {
if (d_stop_state == false)
setStop(true);
else
setStop(false);
}

void DisplaysForm::setGrid(bool on) {
if (on) {
d_grid_state = true;
} else {
d_grid_state = false;
}

    // create one grid per eye pattern
for (unsigned int i = 0; i < d_nplots; ++i) {
if (on) {
    // will auto-detach if already attached.
d_grids[i]->attach(d_displays_plot[i]);
} else {
d_grids[i]->detach();
}
d_grid_act->setChecked(on);
d_displays_plot[i]->replot();
}
}

void DisplaysForm::setAxisLabels(bool en) {
d_axislabels = en;
d_axislabelsmenu->setChecked(en);

for (unsigned int i = 0; i < d_nplots; ++i) {
d_displays_plot[i]->setAxisLabels(d_axislabels);
}
}

void DisplaysForm::saveFigure() {
QPixmap qpix = QPixmap::grabWidget(this);

QString types = QString(tr("JPEG file (*.jpg);;Portable Network Graphics file "
"(*.png);;Bitmap file (*.bmp);;TIFF file (*.tiff)"));

QString filename, filetype;
QFileDialog* filebox = new QFileDialog(0, "Save Image", "./", types);
filebox->setViewMode(QFileDialog::Detail);
if (filebox->exec()) {
filename = filebox->selectedFiles()[0];
filetype = filebox->selectedNameFilter();
} else {
return;
}

if (filetype.contains(".jpg")) {
qpix.save(filename, "JPEG");
} else if (filetype.contains(".png")) {
qpix.save(filename, "PNG");
} else if (filetype.contains(".bmp")) {
qpix.save(filename, "BMP");
} else if (filetype.contains(".tiff")) {
qpix.save(filename, "TIFF");
} else {
qpix.save(filename, "JPEG");
}

delete filebox;
}

void DisplaysForm::disableLegend() {
for (unsigned int i = 0; i < d_nplots; ++i) {
d_displays_plot[i]->disableLegend();
}
}
