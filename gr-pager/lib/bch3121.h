/* -*- c++ -*- */
/*
 * Copyright 2006,2012,2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PAGER_BCH3121_H
#define INCLUDED_PAGER_BCH3121_H

#include "util.h"

namespace gr {
  namespace pager {

    // Perform BCH (31,21) error correction on supplied data
    // Return number of errors found/corrected (0, 1, or 2)
    uint32_t bch3121(uint32_t &data);

  } /* namespace pager */
} /* namespace gr */

#endif /* INCLUDED_PAGER_BCH3121_H */
