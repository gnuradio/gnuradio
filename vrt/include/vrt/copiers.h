/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef INCLUDED_VRT_COPIERS_H
#define INCLUDED_VRT_COPIERS_H

#include <stdint.h>
#include <stddef.h>
#include <complex>

namespace vrt {

  /*!
   * \brief Copy and convert from net format to host format
   */
  void 
  copy_net_16sc_to_host_16sc(size_t nitems,
			     const uint32_t *items,
			     std::complex<int16_t> *host_items);


  /*!
   * \brief Copy and convert from net format to host format mapping [-32768, 32767] -> [1.0, +1.0)
   */
  void
  copy_net_16sc_to_host_32fc(size_t nitems,
			     const uint32_t *items,
			     std::complex<float> *host_items);
};

#endif /* INCLUDED_VRT_COPIERS_H */
