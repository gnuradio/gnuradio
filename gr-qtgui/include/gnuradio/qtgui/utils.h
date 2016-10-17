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

#ifndef INCLUDED_QTGUI_UTILS_H
#define INCLUDED_QTGUI_UTILS_H

#include <qevent.h>
#include <gnuradio/qtgui/api.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <QString>
#include <QCoreApplication>

/*!
 * From QSS file name, extracts the file contents and returns a
 * QString that contains the QSS information. Designed to be passed to
 * the qApplication. The QSS file is typically retrieved using the
 * [qtgui] qss=\<filename\> section of the preferences files.
 */
QTGUI_API QString get_qt_style_sheet(QString filename);

class QTGUI_API QwtDblClickPlotPicker: public QwtPlotPicker
{
public:
#if QWT_VERSION < 0x060100
    QwtDblClickPlotPicker(QwtPlotCanvas *);
#else /* QWT_VERSION < 0x060100 */
    QwtDblClickPlotPicker(QWidget *);
#endif /* QWT_VERSION < 0x060100 */

    ~QwtDblClickPlotPicker();

    virtual QwtPickerMachine * stateMachine(int) const;
};

class QTGUI_API QwtPickerDblClickPointMachine: public QwtPickerMachine
{
public:
  QwtPickerDblClickPointMachine();
  ~QwtPickerDblClickPointMachine();

#if QWT_VERSION < 0x060000
  virtual CommandList
#else
  virtual QList<QwtPickerMachine::Command>
#endif
    transition( const QwtEventPattern &eventPattern,
				  const QEvent *e);
};

void check_set_qss(QApplication *app);

#endif /* INCLUDED_QTGUI_UTILS_H */
