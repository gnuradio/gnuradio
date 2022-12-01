/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/qtgui/utils.h>

#include <gnuradio/prefs.h>

#include <qapplication.h>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

QString get_qt_style_sheet(QString filename)
{
    QString sstext;
    QFile ss(filename);
    if (!ss.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return sstext;
    }

    QTextStream sstream(&ss);
    while (!sstream.atEnd()) {
        sstext += sstream.readLine();
    }
    ss.close();

    return sstext;
}

QwtPickerDblClickPointMachine::QwtPickerDblClickPointMachine()
    : QwtPickerMachine(PointSelection)
{
}

QwtPickerDblClickPointMachine::~QwtPickerDblClickPointMachine() {}

QList<QwtPickerMachine::Command>
QwtPickerDblClickPointMachine::transition(const QwtEventPattern& eventPattern,
                                          const QEvent* e)
{
    QList<QwtPickerMachine::Command> cmdList;
    switch (e->type()) {
    case QEvent::MouseButtonDblClick:
        if (eventPattern.mouseMatch(QwtEventPattern::MouseSelect1,
                                    (const QMouseEvent*)e)) {
            cmdList += QwtPickerMachine::Begin;
            cmdList += QwtPickerMachine::Append;
            cmdList += QwtPickerMachine::End;
        }
        break;
    default:
        break;
    }
    return cmdList;
}

QwtDblClickPlotPicker::QwtDblClickPlotPicker(QWidget* canvas) : QwtPlotPicker(canvas) {}

QwtDblClickPlotPicker::~QwtDblClickPlotPicker() {}

QwtPickerMachine* QwtDblClickPlotPicker::stateMachine(int n) const
{
    return new QwtPickerDblClickPointMachine;
}

void check_set_qss(QApplication* app)
{
    std::string qssfile = gr::prefs::singleton()->get_string("qtgui", "qss", "");
    if (!qssfile.empty()) {
        QString sstext = get_qt_style_sheet(QString(qssfile.c_str()));
        app->setStyleSheet(sstext);
    }
}
