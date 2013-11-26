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

#ifndef INCLUDED_QTGUI_TRIGGER_MODE_H
#define INCLUDED_QTGUI_TRIGGER_MODE_H

namespace gr {
  namespace qtgui {

    enum trigger_mode {
      TRIG_MODE_FREE,
      TRIG_MODE_AUTO,
      TRIG_MODE_NORM,
      TRIG_MODE_TAG,
    };

    enum trigger_slope {
      TRIG_SLOPE_POS,
      TRIG_SLOPE_NEG,
    };

  } /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_TRIGGER_MODE_H */
