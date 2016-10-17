/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

#include <gnuradio/qtgui/utils.h>
#include <gnuradio/prefs.h>
#include <QDebug>
#include <QFile>
#include <QCoreApplication>
#include <qapplication.h>

QString
get_qt_style_sheet(QString filename)
{
  QString sstext;
  QFile ss(filename);
  if(!ss.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return sstext;
  }

  QTextStream sstream(&ss);
  while(!sstream.atEnd()) {
    sstext += sstream.readLine();
  }
  ss.close();

  return sstext;
}

QwtPickerDblClickPointMachine::QwtPickerDblClickPointMachine()
#if QWT_VERSION < 0x060000
    : QwtPickerMachine ()
#else
    : QwtPickerMachine (PointSelection)
#endif
{
}

QwtPickerDblClickPointMachine::~QwtPickerDblClickPointMachine()
{
}

#if QWT_VERSION < 0x060000
#define CMDLIST_TYPE QwtPickerMachine::CommandList
#else
#define CMDLIST_TYPE QList<QwtPickerMachine::Command>
#endif
CMDLIST_TYPE
QwtPickerDblClickPointMachine::transition(const QwtEventPattern &eventPattern,
					  const QEvent *e)
{
  CMDLIST_TYPE cmdList;
  switch(e->type()) {
    case QEvent::MouseButtonDblClick:
      if ( eventPattern.mouseMatch(QwtEventPattern::MouseSelect1,
				   (const QMouseEvent *)e) ) {
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
#else /* QWT_VERSION < 0x060100 */
QwtDblClickPlotPicker::QwtDblClickPlotPicker(QWidget* canvas)
#endif /* QWT_VERSION < 0x060100 */
  : QwtPlotPicker(canvas)
{
#if QWT_VERSION < 0x060000
  setSelectionFlags(QwtPicker::PointSelection);
#endif
}

QwtDblClickPlotPicker::~QwtDblClickPlotPicker()
{
}

QwtPickerMachine*
QwtDblClickPlotPicker::stateMachine(int n) const
{
  return new QwtPickerDblClickPointMachine;
}

void check_set_qss(QApplication *app){
      std::string qssfile = gr::prefs::singleton()->get_string("qtgui","qss","");
      if(qssfile.size() > 0) {
        QString sstext = get_qt_style_sheet(QString(qssfile.c_str()));
        app->setStyleSheet(sstext);
      }
}

