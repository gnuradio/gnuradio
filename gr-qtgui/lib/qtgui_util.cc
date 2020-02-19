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
#if QWT_VERSION < 0x060000
    : QwtPickerMachine()
#else
    : QwtPickerMachine(PointSelection)
#endif
{
}

QwtPickerDblClickPointMachine::~QwtPickerDblClickPointMachine() {}

#if QWT_VERSION < 0x060000
#define CMDLIST_TYPE QwtPickerMachine::CommandList
#else
#define CMDLIST_TYPE QList<QwtPickerMachine::Command>
#endif
CMDLIST_TYPE
QwtPickerDblClickPointMachine::transition(const QwtEventPattern& eventPattern,
                                          const QEvent* e)
{
    CMDLIST_TYPE cmdList;
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

#if QWT_VERSION < 0x060100
QwtDblClickPlotPicker::QwtDblClickPlotPicker(QwtPlotCanvas* canvas)
#else  /* QWT_VERSION < 0x060100 */
QwtDblClickPlotPicker::QwtDblClickPlotPicker(QWidget* canvas)
#endif /* QWT_VERSION < 0x060100 */
    : QwtPlotPicker(canvas)
{
#if QWT_VERSION < 0x060000
    setSelectionFlags(QwtPicker::PointSelection);
#endif
}

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
