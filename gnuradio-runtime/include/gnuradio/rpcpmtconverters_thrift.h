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

#ifndef RPCPMTCONVERTERS_THRIFT_H
#define RPCPMTCONVERTERS_THRIFT_H

#include <pmt/pmt.h>
#include "gnuradio_types.h"

namespace GNURadio {
  typedef boost::shared_ptr<Knob> KnobPtr;
}

namespace rpcpmtconverter
{
  pmt::pmt_t to_pmt(const GNURadio::Knob& knob);
  GNURadio::Knob from_pmt(const pmt::pmt_t& knob);
}

#endif /* RPCPMTCONVERTERS_THRIFT_H */
