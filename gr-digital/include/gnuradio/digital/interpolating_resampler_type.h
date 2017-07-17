/* -*- c++ -*- */
/*
 * Copyright (C) 2017 Free Software Foundation, Inc.
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
 * along with this file; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DIGITAL_INTERPOLATING_RESAMPLER_TYPE_H
#define INCLUDED_DIGITAL_INTERPOLATING_RESAMPLER_TYPE_H

namespace gr {
  namespace digital {

    // Interpolating Resampler type
    enum ir_type {
      IR_NONE      = -1,
      IR_MMSE_8TAP = 0,  // Valid for [-Fs/4, Fs/4] bandlimited input
      IR_PFB_NO_MF = 1,  // No matched filtering, just interpolation
      IR_PFB_MF    = 2,
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_INTERPOLATING_RESAMPLER_TYPE_H */
