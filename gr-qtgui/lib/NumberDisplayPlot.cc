/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef NUMBER_DISPLAY_PLOT_C
#define NUMBER_DISPLAY_PLOT_C

#include <gnuradio/qtgui/NumberDisplayPlot.h>

#include <qwt_scale_draw.h>
#include <QColor>
#include <cmath>
#include <iostream>
#include <volk/volk.h>

NumberDisplayPlot::NumberDisplayPlot(int nplots, QWidget* parent)
  : DisplayPlot(nplots, parent)
{
  resize(0, 0);

  // Setup dataPoints and plot vectors
  // Automatically deleted when parent is deleted
  for(int i = 0; i < d_nplots; i++) {

  }
}

NumberDisplayPlot::~NumberDisplayPlot()
{
}

void
NumberDisplayPlot::replot()
{
  QwtPlot::replot();
}

void
NumberDisplayPlot::plotNewData(const std::vector<double> samples)
{
  if(!d_stop) {
  }
}

#endif /* NUMBER_DISPLAY_PLOT_C */
