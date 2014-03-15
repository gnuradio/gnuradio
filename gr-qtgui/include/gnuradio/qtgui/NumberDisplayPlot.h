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

#ifndef NUMBER_DISPLAY_PLOT_H
#define NUMBER_DISPLAY_PLOT_H

#include <stdint.h>
#include <cstdio>
#include <vector>
#include <gnuradio/qtgui/DisplayPlot.h>
#include <gnuradio/tags.h>
#include <qwt_plot.h>

/*!
 * \brief QWidget for displaying number plots.
 * \ingroup qtgui_blk
 */
class NumberDisplayPlot: public DisplayPlot
{
  Q_OBJECT

public:
  NumberDisplayPlot(int nplots, QWidget*);
  virtual ~NumberDisplayPlot();

  void plotNewData(const std::vector<double> samples);

  void replot();

public slots:

private:
};

#endif /* NUMBER_DISPLAY_PLOT_H */
