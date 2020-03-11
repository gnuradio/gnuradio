/* -*- c++ -*- */
/*
 * Copyright 2011,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_UTILS_H
#define INCLUDED_QTGUI_UTILS_H

#include <gnuradio/qtgui/api.h>
#include <qevent.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_picker.h>
#include <QCoreApplication>
#include <QString>

/*!
 * From QSS file name, extracts the file contents and returns a
 * QString that contains the QSS information. Designed to be passed to
 * the qApplication. The QSS file is typically retrieved using the
 * [qtgui] qss=\<filename\> section of the preferences files.
 */
QTGUI_API QString get_qt_style_sheet(QString filename);

class QTGUI_API QwtDblClickPlotPicker : public QwtPlotPicker
{
public:
#if QWT_VERSION < 0x060100
    QwtDblClickPlotPicker(QwtPlotCanvas*);
#else  /* QWT_VERSION < 0x060100 */
    QwtDblClickPlotPicker(QWidget*);
#endif /* QWT_VERSION < 0x060100 */

    ~QwtDblClickPlotPicker();

    virtual QwtPickerMachine* stateMachine(int) const;
};

class QTGUI_API QwtPickerDblClickPointMachine : public QwtPickerMachine
{
public:
    QwtPickerDblClickPointMachine();
    ~QwtPickerDblClickPointMachine();

#if QWT_VERSION < 0x060000
    virtual CommandList
#else
    virtual QList<QwtPickerMachine::Command>
#endif
    transition(const QwtEventPattern& eventPattern, const QEvent* e);
};

void check_set_qss(QApplication* app);

#endif /* INCLUDED_QTGUI_UTILS_H */
