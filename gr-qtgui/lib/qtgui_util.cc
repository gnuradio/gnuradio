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

#include <qtgui_util.h>

QwtPickerDblClickPointMachine::QwtPickerDblClickPointMachine()
    : QwtPickerMachine ()
{

}

QwtPickerDblClickPointMachine::~QwtPickerDblClickPointMachine()
{

}

QwtPickerMachine::CommandList
QwtPickerDblClickPointMachine::transition(const QwtEventPattern &eventPattern,
					  const QEvent *e)
{
  QwtPickerMachine::CommandList cmdList;
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

QwtDblClickPlotPicker::QwtDblClickPlotPicker(QwtPlotCanvas* canvas)
  : QwtPlotPicker(canvas)
{
  setSelectionFlags(QwtPicker::PointSelection);
}

QwtDblClickPlotPicker::~QwtDblClickPlotPicker()
{
}

QwtPickerMachine*
QwtDblClickPlotPicker::stateMachine(int n) const
{
  return new QwtPickerDblClickPointMachine;
}

